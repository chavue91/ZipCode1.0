/// @file SearchIndex.cpp
/// @brief Searches for ZIP codes in a blocked sequence set using a primary key index file.

#include "HeaderBuffer3.h"
#include "BlockBuffer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

/// @brief Extracts the primary key (ZIP code) from a record.
string extractKey(const string& record) {
    stringstream ss(record);
    string key;
    getline(ss, key, ',');
    return key;
}

/// @brief Loads the index file into a sorted map.
map<string, int> loadIndex(const string& indexFile) {
    map<string, int> index;
    ifstream in(indexFile);
    if (!in.is_open()) {
        cerr << "Error opening index file." << endl;
        return index;
    }
    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string key;
        int rbn;
        getline(ss, key, ',');
        ss >> rbn;
        index[key] = rbn;
    }
    return index;
}

/// @brief Finds the smallest RBN whose key is >= searchKey.
int locateBlock(const map<string, int>& index, const string& searchKey) {
    auto it = index.lower_bound(searchKey);
    if (it == index.end()) return -1;
    return it->second;
}

/// @brief Entry point to search ZIP codes using -z##### flags.
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <blocked_file> <index_file> -z##### [-z##### ...]" << endl;
        return 1;
    }

    string blockedFile = argv[1];
    string indexFile = argv[2];
    vector<string> searchZips;

    for (int i = 3; i < argc; ++i) {
        if (strncmp(argv[i], "-z", 2) == 0 || strncmp(argv[i], "-Z", 2) == 0) {
            searchZips.push_back(argv[i] + 2);
        }
    }

    auto index = loadIndex(indexFile);
    if (index.empty()) return 1;

    ifstream data(blockedFile, ios::binary);
    if (!data.is_open()) {
        cerr << "Could not open blocked file." << endl;
        return 1;
    }

    HeaderBuffer3 hb;
    if (!hb.read(data)) {
        cerr << "Failed to read header from blocked file." << endl;
        return 1;
    }
    int blockSize = hb.header.blockSize;

    for (const string& zip : searchZips) {
        int rbn = locateBlock(index, zip);
        if (rbn == -1) {
            cout << "Zip Code " << zip << " not found in index." << endl;
            continue;
        }

        Block blk;
        if (!BlockBuffer::readBlock(data, blk, rbn, blockSize)) {
            cerr << "Failed to read block RBN " << rbn << endl;
            continue;
        }

        bool found = false;
        for (const auto& record : blk.records) {
            if (extractKey(record) == zip) {
                cout << "Found: " << record << endl;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Zip Code " << zip << " not found in data block." << endl;
        }
    }

    return 0;
}