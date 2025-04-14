/// @file BuildIndex.cpp
/// @brief Builds a simple primary key index file from a blocked sequence set.

#include "HeaderBuffer3.h"
#include "BlockBuffer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

/// @brief Extracts the primary key (ZIP code) from a record string.
string extractKey(const string& record) {
    stringstream ss(record);
    string key;
    getline(ss, key, ',');
    return key;
}

/// @brief Entry point to build an index file from a blocked sequence set.
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <blocked_file> <output_index_file>" << endl;
        return 1;
    }

    string blockedFile = argv[1];
    string indexFile = argv[2];

    ifstream in(blockedFile, ios::binary);
    if (!in.is_open()) {
        cerr << "Error opening blocked file." << endl;
        return 1;
    }

    HeaderBuffer3 hb;
    if (!hb.read(in)) {
        cerr << "Failed to read header." << endl;
        return 1;
    }

    int blockSize = hb.header.blockSize;
    int blockCount = hb.header.blockCount;

    if (blockSize <= 0 || blockCount <= 0) {
        cerr << "Invalid block size or block count in header." << endl;
        return 1;
    }

    vector<pair<string, int>> index; ///< Vector of {highest key in block, RBN}

    for (int rbn = 0; rbn < blockCount; ++rbn) {
        in.clear();
        in.seekg(0); // reset stream in case it's in a failed state

        Block blk;
        if (!BlockBuffer::readBlock(in, blk, rbn, blockSize)) {
            cerr << "Warning: Could not read block " << rbn << endl;
            continue;
        }

        if (blk.recordCount == 0 || blk.records.empty()) continue;

        vector<string> keys;
        for (const auto& rec : blk.records) {
            if (!rec.empty()) {
                keys.push_back(extractKey(rec));
            }
        }

        if (keys.empty()) continue;

        sort(keys.begin(), keys.end());
        string maxKey = keys.back();
        index.emplace_back(maxKey, rbn);
    }

    in.close();
    ofstream out(indexFile);
    if (!out.is_open()) {
        cerr << "Error opening output index file." << endl;
        return 1;
    }

    for (const auto& [key, rbn] : index) {
        out << key << "," << rbn << endl;
    }

    cout << "Index file generated: " << indexFile << endl;
    return 0;
}
