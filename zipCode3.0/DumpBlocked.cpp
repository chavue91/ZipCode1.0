/// @file DumpBlocked.cpp
/// @brief Utility to dump blocked sequence set content in physical or logical order.

#include "HeaderBuffer3.h"
#include "BlockBuffer.h"
#include <fstream>
#include <iostream>
#include <unordered_set>

using namespace std;

/// @brief Dumps all blocks in physical order.
void dumpPhysical(const string& filename, int blockSize, int blockCount) {
    ifstream in(filename, ios::binary);
    if (!in.is_open()) {
        cerr << "Error opening file for physical dump." << endl;
        return;
    }

    cout << "\n--- Physical Dump ---\n";
    for (int rbn = 0; rbn < blockCount; ++rbn) {
        Block blk;
        BlockBuffer::readBlock(in, blk, rbn, blockSize);

        if (blk.recordCount == 0) {
            cout << "RBN " << rbn << ": *available*" << endl;
        } else {
            cout << "RBN " << rbn << ": ";
            for (const auto& rec : blk.records) {
                string key = rec.substr(5, 5); // assume ZIP is field 0, 5 digits
                cout << key << " ";
            }
            cout << "→ RBN " << blk.nextRBN << endl;
        }
    }
    in.close();
}

/// @brief Dumps blocks in logical sequence order based on header links.
void dumpLogical(const string& filename, int blockSize, int startRBN) {
    ifstream in(filename, ios::binary);
    if (!in.is_open()) {
        cerr << "Error opening file for logical dump." << endl;
        return;
    }

    cout << "\n--- Logical Dump ---\n";
    int current = startRBN;
    unordered_set<int> visited;

    while (current != -1 && visited.find(current) == visited.end()) {
        visited.insert(current);
        Block blk;
        BlockBuffer::readBlock(in, blk, current, blockSize);

        if (blk.recordCount == 0) {
            cout << "RBN " << current << ": *available*" << endl;
        } else {
            cout << "RBN " << current << ": ";
            for (const auto& rec : blk.records) {
                string key = rec.substr(5, 5); // assume ZIP is field 0
                cout << key << " ";
            }
            cout << "→ RBN " << blk.nextRBN << endl;
        }
        current = blk.nextRBN;
    }
    in.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <blocked_file> <physical|logical>" << endl;
        return 1;
    }

    string filename = argv[1];
    string mode = argv[2];

    ifstream in(filename, ios::binary);
    HeaderBuffer3 hb;
    if (!hb.read(in)) {
        cerr << "Failed to read header." << endl;
        return 1;
    }
    in.close();

    if (mode == "physical") {
        dumpPhysical(filename, hb.header.blockSize, hb.header.blockCount);
    } else if (mode == "logical") {
        dumpLogical(filename, hb.header.blockSize, hb.header.sequenceListHeadRBN);
    } else {
        cerr << "Invalid mode. Use 'physical' or 'logical'." << endl;
        return 1;
    }

    return 0;
}