/// @file BlockBuffer.cpp
/// @brief Implements BlockBuffer class functions for handling block-level I/O.

#include "BlockBuffer.h"
#include <sstream>

using namespace std;

bool BlockBuffer::writeBlock(fstream& out, const Block& block, int rbn, int blockSize) {
    if (!out.is_open()) return false;
    streampos pos = static_cast<streampos>(rbn) * blockSize;
    out.seekp(pos);

    stringstream buffer;
    buffer << block.recordCount << '\n';
    buffer << block.prevRBN << '\n';
    buffer << block.nextRBN << '\n';
    for (const auto& rec : block.records) {
        buffer << rec << '\n';
    }

    string rawBlock = buffer.str();
    if (rawBlock.size() > static_cast<size_t>(blockSize)) return false;
    rawBlock.resize(blockSize, ' ');
    out.write(rawBlock.c_str(), blockSize);
    return true;
}

bool BlockBuffer::readBlock(istream& in, Block& block, int rbn, int blockSize) {
    if (!in) return false;
    streampos pos = static_cast<streampos>(rbn) * blockSize;
    in.seekg(pos);

    vector<char> buffer(blockSize);
    in.read(buffer.data(), blockSize);
    stringstream ss(string(buffer.data(), blockSize));

    ss >> block.recordCount;
    ss.ignore();
    ss >> block.prevRBN;
    ss.ignore();
    ss >> block.nextRBN;
    ss.ignore();

    block.records.clear();
    string line;
    for (int i = 0; i < block.recordCount && getline(ss, line); ++i) {
        block.records.push_back(line);
    }

    return true;
}

void BlockBuffer::writeAvailBlock(ostream& out, const AvailBlock& avail, int rbn, int blockSize) {
    streampos pos = static_cast<streampos>(rbn) * blockSize;
    out.seekp(pos);

    stringstream buffer;
    buffer << avail.recordCount << '\n';
    buffer << avail.nextAvailRBN << '\n';
    string data = buffer.str();
    data += avail.blankFill;
    data.resize(blockSize, ' ');

    out.write(data.c_str(), blockSize);
}

int BlockBuffer::readNextAvailRBN(ifstream& in, int rbn, int blockSize) {
    if (!in.is_open()) return -1;
    streampos pos = static_cast<streampos>(rbn) * blockSize;
    in.seekg(pos);

    int recordCount;
    int nextRBN;
    in >> recordCount;
    in.ignore();
    in >> nextRBN;
    return nextRBN;
}