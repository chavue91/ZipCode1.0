/// @file ConvertToBlocked.cpp
/// @brief Converts a length-indicated file into a blocked sequence set format.

#include "HeaderBuffer3.h"
#include "BlockBuffer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>

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

    ofstream out(outputFile, ios::binary);
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
    float minCapacity = hb.header.minBlockCapacity;
    int minRecordsPerBlock = static_cast<int>(ceil(minCapacity * (blockSize / 64.0))); // rough guess per record size

    vector<string> buffer;
    string line;
    int currentRBN = 0;
    int sequenceHead = 0;
    int prevRBN = -1;
    int blockCount = 0;

    // Prepare to skip header lines in LI file
    int headerLinesToSkip = 0;
    getline(in, line);
    while (!line.empty() && !isdigit(line[0])) {
        getline(in, line);
        headerLinesToSkip++;
    }
    in.clear();
    in.seekg(0);
    for (int i = 0; i < headerLinesToSkip; ++i) getline(in, line);

    // Read and build blocks
    while (getline(in, line)) {
        if (line.empty()) continue;
        buffer.push_back(line);

        if (buffer.size() >= static_cast<size_t>(minRecordsPerBlock)) {
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
        }
    }

    // Write final block if needed
    if (!buffer.empty()) {
        Block blk;
        blk.recordCount = buffer.size();
        blk.records = buffer;
        blk.prevRBN = prevRBN;
        blk.nextRBN = -1;
        BlockBuffer::writeBlock(out, blk, currentRBN, blockSize);
        ++blockCount;
    }

    // Update and write header
    hb.header.sequenceListHeadRBN = 0;
    hb.header.blockCount = blockCount;
    out.seekp(0);
    hb.write(out);

    cout << "Blocked sequence set file generated: " << outputFile << endl;
    return 0;
}