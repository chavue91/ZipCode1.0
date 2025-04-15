/// @file ConvertToBlocked.cpp
/// @brief Converts a length-indicated file into a blocked sequence set format.

#include "HeaderBuffer3.h"
#include "BlockBuffer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>
#include <stdexcept>

using namespace std;

/// @brief Entry point for the CSV-to-blocked conversion tool.
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_length_indicated_file> <output_blocked_file>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];

    ifstream in(inputFile);
    if (!in.is_open()) {
        cerr << "Error: Could not open input file." << endl;
        return 1;
    }

    fstream out(outputFile, ios::in | ios::out | ios::binary | ios::trunc);
    if (!out.is_open()) {
        cerr << "Error: Could not create output file." << endl;
        return 1;
    }

    // Read header
    HeaderBuffer3 hb;
    if (!hb.read(in)) {
        cerr << "Error: Failed to read header from input file." << endl;
        return 1;
    }

    int blockSize = hb.header.blockSize;
    if (blockSize <= 0 || blockSize > 512 * 1024) {
        cerr << "Invalid or suspicious block size: " << blockSize << endl;
        return 1;
    }

    vector<string> buffer;
    string line;
    int currentRBN = 0;
    int prevRBN = -1;
    int blockCount = 0;

    // Reserve space for header padded to full block size
    stringstream dummyHeader;
    hb.write(dummyHeader);
    string headerText = dummyHeader.str();

    string paddedHeader;
    try {
        paddedHeader = headerText;
        if (paddedHeader.size() > static_cast<size_t>(blockSize)) {
            cerr << "Error: Header size exceeds block size." << endl;
            return 1;
        }
        paddedHeader.resize(blockSize, ' ');  // pad with spaces
    } catch (const length_error& e) {
        cerr << "Error resizing padded header: " << e.what() << endl;
        return 1;
    }

    out.seekp(0);
    out.write(paddedHeader.c_str(), blockSize);

    // Skip metadata header lines (non-data)
    while (getline(in, line)) {
        if (!line.empty() && isdigit(line[0])) break;
    }
    if (!in) {
        cerr << "Error: No data lines found after header." << endl;
        return 1;
    }

    // Start writing data blocks
    size_t currentSize = 0;
    do {
        if (line.empty()) continue;
        size_t lineSize = line.size() + 1;
        if (currentSize + lineSize > static_cast<size_t>(blockSize)) {
            Block blk;
            blk.recordCount = buffer.size();
            blk.records = buffer;
            blk.prevRBN = prevRBN;
            blk.nextRBN = currentRBN + 1;

            BlockBuffer::writeBlock(out, blk, currentRBN, blockSize);
            prevRBN = currentRBN;
            ++currentRBN;
            ++blockCount;
            buffer.clear();
            currentSize = 0;
        }
        buffer.push_back(line);
        currentSize += lineSize;
    } while (getline(in, line));

    // Write remaining records
    if (!buffer.empty()) {
        Block blk;
        blk.recordCount = buffer.size();
        blk.records = buffer;
        blk.prevRBN = prevRBN;
        blk.nextRBN = -1;
        BlockBuffer::writeBlock(out, blk, currentRBN, blockSize);
        ++blockCount;
    }

    // Update header and write it back
    hb.header.sequenceListHeadRBN = 0;
    hb.header.blockCount = blockCount;
    out.seekp(0);
    hb.write(out);

    cout << "Blocked sequence set file generated: " << outputFile << endl;
    return 0;
}
