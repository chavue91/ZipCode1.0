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
    const int MAX_BLOCKS = 50000; // Cap to prevent runaway memory

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

    HeaderBuffer3 hb;
    if (!hb.read(in)) {
        cerr << "Error: Failed to read header from input file." << endl;
        return 1;
    }

    int blockSize = 1024 * 4; // 4 KB blocks
    if (hb.header.blockSize > 0) {
        blockSize = hb.header.blockSize;
    }
    hb.header.blockSize = blockSize;
    if (blockSize < 512) {
        cerr << "Error: Block size must be at least 512 bytes." << endl;
        return 1;
    }

    vector<string> buffer;
    string line;
    int currentRBN = 0;
    int prevRBN = -1;
    int blockCount = 0;
    int sequenceListHeadRBN = -1;

    stringstream dummyHeader;
    hb.write(dummyHeader);
    string headerText = dummyHeader.str();

    if (headerText.size() > static_cast<size_t>(blockSize)) {
        cerr << "Error: Header size exceeds block size." << endl;
        return 1;
    }

    string paddedHeader = headerText;
    paddedHeader.resize(blockSize, ' ');

    out.seekp(0);
    out.write(paddedHeader.c_str(), blockSize);

    int totalHeaderLines = 1 + hb.header.fieldsPerRecord;
    for (int i = 0; i < totalHeaderLines; ++i) {
        if (!getline(in, line)) {
            cerr << "Error: Failed to skip header lines." << endl;
            return 1;
        }
    }

    size_t currentSize = 0;
    int lineCount = 0;
    while (getline(in, line)) {
        ++lineCount;
        if (line.empty()) continue;

        if (line.find(',') == string::npos) {
            cerr << "[Error] Skipping malformed line: " << line << endl;
            continue;
        }

        size_t lineSize = line.size() + 1;
        if (lineSize > static_cast<size_t>(blockSize)) {
            cerr << "Error: Line too large for block." << endl;
            return 1;
        }

        if (currentSize + lineSize > static_cast<size_t>(blockSize)) {
            if (blockCount >= MAX_BLOCKS) {
                cerr << "Error: Max block count exceeded (" << MAX_BLOCKS << "). Aborting." << endl;
                return 1;
            }

            Block blk;
            blk.recordCount = buffer.size();
            blk.records = buffer;
            blk.prevRBN = prevRBN;
            blk.nextRBN = -1; // set to -1 temporarily

            stringstream blockStream;
            blockStream << blk.recordCount << '\n';
            blockStream << blk.prevRBN << '\n';
            blockStream << blk.nextRBN << '\n';
            for (const auto& rec : blk.records) blockStream << rec << '\n';
            string rawBlock = blockStream.str();
            if (rawBlock.size() > static_cast<size_t>(blockSize)) {
                cerr << "Error: Block content exceeds block size." << endl;
                return 1;
            }

            BlockBuffer::writeBlock(out, blk, currentRBN, blockSize);

            if (sequenceListHeadRBN == -1) {
                sequenceListHeadRBN = currentRBN;
            }

            prevRBN = currentRBN;
            ++currentRBN;
            ++blockCount;
            buffer.clear();
            currentSize = 0;
        }

        buffer.push_back(line);
        currentSize += lineSize;

        if (lineCount % 5000 == 0) {
            cout << "[Debug] Processed " << lineCount << " lines..." << endl;
        }
    }

    if (!buffer.empty()) {
        Block blk;
        blk.recordCount = buffer.size();
        blk.records = buffer;
        blk.prevRBN = prevRBN;
        blk.nextRBN = -1;

        stringstream blockStream;
        blockStream << blk.recordCount << '\n';
        blockStream << blk.prevRBN << '\n';
        blockStream << blk.nextRBN << '\n';
        for (const auto& rec : blk.records) blockStream << rec << '\n';
        string rawBlock = blockStream.str();
        if (rawBlock.size() > static_cast<size_t>(blockSize)) {
            cerr << "Error: Last block content exceeds block size." << endl;
            return 1;
        }

        BlockBuffer::writeBlock(out, blk, currentRBN, blockSize);
        if (sequenceListHeadRBN == -1) {
            sequenceListHeadRBN = currentRBN;
        }
        ++blockCount;
    }

    hb.header.sequenceListHeadRBN = sequenceListHeadRBN;
    hb.header.blockCount = blockCount;

    stringstream finalHeader;
    hb.write(finalHeader);
    string headerFinal = finalHeader.str();

    if (headerFinal.size() > static_cast<size_t>(blockSize)) {
        cerr << "Error: Final header size exceeds block size." << endl;
        return 1;
    }

    headerFinal.resize(blockSize, ' ');
    out.seekp(0);
    out.write(headerFinal.c_str(), blockSize);

    cout << "[Info] Total blocks written: " << blockCount << endl;
    cout << "Blocked sequence set file generated: " << outputFile << endl;
    return 0;
}