/// @file DumpUtility.cpp
/// @brief Implements dump functions for the blocked sequence set structure.

#include "DumpUtility.h"
#include "HeaderBuffer3.h"
#include "BlockBuffer.h"
#include "RecordBuffer.h"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

void printBlock(int rbn, const Block& block) {
    cout << rbn << "  ";
    for (const string& rec : block.records) {
        RecordBuffer rb(rec);
        cout << rb.getKey() << " ";
    }
    cout << "  " << block.nextRBN << endl;
}

void printAvail(int rbn, const AvailBlock& avail) {
    cout << rbn << "  *available*    " << avail.nextAvailRBN << endl;
}

void dumpByPhysicalOrder(const string& filename) {
    ifstream in(filename, ios::in | ios::binary);
    if (!in.is_open()) {
        cerr << "Could not open file: " << filename << endl;
        return;
    }

    HeaderBuffer3 hb;
    if (!hb.read(in)) {
        cerr << "Failed to read header." << endl;
        return;
    }

    int blockSize = hb.header.blockSize;
    cout << "List Head:  " << hb.header.sequenceListHeadRBN << endl;
    cout << "Avail Head: " << hb.header.availListHeadRBN << endl;

    for (int rbn = 0; rbn < hb.header.blockCount; ++rbn) {
        in.clear();
        in.seekg(0);

        Block blk;
        if (BlockBuffer::readBlock(in, blk, rbn, blockSize)) {
            if (blk.recordCount == 0) {
                AvailBlock avail(BlockBuffer::readNextAvailRBN(in, rbn, blockSize));
                printAvail(rbn, avail);
            } else {
                printBlock(rbn, blk);
            }
        }
    }
}

void dumpByLogicalOrder(const string& filename) {
    fstream in(filename, ios::in | ios::binary);
    if (!in.is_open()) {
        cerr << "Could not open file: " << filename << endl;
        return;
    }

    HeaderBuffer3 hb;
    if (!hb.read(in)) {
        cerr << "Failed to read header." << endl;
        return;
    }

    int blockSize = hb.header.blockSize;
    cout << "List Head:  " << hb.header.sequenceListHeadRBN << endl;
    cout << "Avail Head: " << hb.header.availListHeadRBN << endl;

    int rbn = hb.header.sequenceListHeadRBN;
    while (rbn != -1) {
        Block blk;
        if (BlockBuffer::readBlock(in, blk, rbn, blockSize)) {
            printBlock(rbn, blk);
            rbn = blk.nextRBN;
        } else {
            break;
        }
    }
}
