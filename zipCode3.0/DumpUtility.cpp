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

/**
 * @brief Prints a block’s RBN, keys, and next RBN.
 * @param rbn Relative Block Number.
 * @param block The Block structure to print.
 */
void printBlock(int rbn, const Block& block) {
    cout << rbn << "  ";
    for (const string& rec : block.records) {
        RecordBuffer rb(rec);               // Extract fields using record buffer
        cout << rb.getKey() << " ";         // Print key field
    }
    cout << "  " << block.nextRBN << endl;  // Show next RBN link
}

/**
 * @brief Prints an avail block's RBN and its successor.
 * @param rbn Relative Block Number of the avail block.
 * @param avail The AvailBlock to print.
 */
void printAvail(int rbn, const AvailBlock& avail) {
    cout << rbn << "  *available*    " << avail.nextAvailRBN << endl;
}

/**
 * @brief Dumps all blocks in physical order from the file.
 * @param filename The blocked sequence set file.
 */
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

    // Print header pointers
    cout << "List Head:  " << hb.header.sequenceListHeadRBN << endl;
    cout << "Avail Head: " << hb.header.availListHeadRBN << endl;

    // Traverse blocks sequentially by physical RBN
    for (int rbn = 0; rbn < hb.header.blockCount; ++rbn) {
        in.clear();             // Clear EOF/failure flags
        in.seekg(0);            // Reset file position

        Block blk;
        if (BlockBuffer::readBlock(in, blk, rbn, blockSize)) {
            if (blk.recordCount == 0) {
                // Handle avail block
                AvailBlock avail(BlockBuffer::readNextAvailRBN(in, rbn, blockSize));
                printAvail(rbn, avail);
            } else {
                printBlock(rbn, blk);
            }
        }
    }
}

/**
 * @brief Dumps all blocks in logical order using nextRBN links.
 * @param filename The blocked sequence set file.
 */
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

    // Print header pointers
    cout << "List Head:  " << hb.header.sequenceListHeadRBN << endl;
    cout << "Avail Head: " << hb.header.availListHeadRBN << endl;

    // Follow logical linked list using nextRBN
    int rbn = hb.header.sequenceListHeadRBN;
    while (rbn != -1) {
        Block blk;
        if (BlockBuffer::readBlock(in, blk, rbn, blockSize)) {
            printBlock(rbn, blk);
            rbn = blk.nextRBN;  // Move to next logical block
        } else {
            break;              // Stop on read failure
        }
    }
}
