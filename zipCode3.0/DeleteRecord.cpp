/// @file DeleteRecord.cpp
/// @brief Handles deletion of ZIP code records from a blocked sequence set.

#include "HeaderBuffer3.h"
#include "BlockBuffer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <set>

using namespace std;

string extractKey(const string& record) {
    stringstream ss(record);
    string key;
    getline(ss, key, ',');
    return key;
}

map<string, int> loadIndex(const string& indexFile) {
    map<string, int> index;
    ifstream in(indexFile);
    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string key; int rbn;
        getline(ss, key, ',');
        ss >> rbn;
        index[key] = rbn;
    }
    return index;
}

void writeIndex(const string& indexFile, const map<string, int>& index) {
    ofstream out(indexFile);
    for (const auto& [key, rbn] : index) {
        out << key << "," << rbn << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <blocked_file> <index_file> <delete_keys_file>" << endl;
        return 1;
    }

    string blockedFile = argv[1];
    string indexFile = argv[2];
    string deleteFile = argv[3];

    map<string, int> index = loadIndex(indexFile);
    if (index.empty()) {
        cerr << "Empty or missing index file." << endl;
        return 1;
    }

    fstream data(blockedFile, ios::in | ios::out | ios::binary);
    if (!data.is_open()) {
        cerr << "Failed to open blocked file." << endl;
        return 1;
    }

    HeaderBuffer3 hb;
    if (!hb.read(data)) {
        cerr << "Failed to read header." << endl;
        return 1;
    }

    int blockSize = hb.header.blockSize;
    set<string> deleted;

    ifstream delFile(deleteFile);
    string zip;
    while (getline(delFile, zip)) {
        auto it = index.lower_bound(zip);
        if (it == index.end()) {
            cout << "Zip Code " << zip << " not found in index." << endl;
            continue;
        }

        int rbn = it->second;
        Block blk;
        if (!BlockBuffer::readBlock(data, blk, rbn, blockSize)) continue;

        auto oldSize = blk.records.size();
        blk.records.erase(remove_if(blk.records.begin(), blk.records.end(), [&](const string& rec) {
            return extractKey(rec) == zip;
        }), blk.records.end());

        if (blk.records.empty()) {
            blk.recordCount = 0;
            {
                fstream outFile(blockedFile, ios::in | ios::out | ios::binary);
                outFile.seekp(data.tellp());
                BlockBuffer::writeAvailBlock(outFile, AvailBlock(blockSize), rbn, blockSize);
            }
            index.erase(it);
            cout << "Deleted entire block RBN " << rbn << endl;
        } else {
            blk.recordCount = blk.records.size();
            {
                fstream outFile(blockedFile, ios::in | ios::out | ios::binary);
                outFile.seekp(data.tellp());
                BlockBuffer::writeBlock(outFile, blk, rbn, blockSize);
            }

            string newMaxKey = extractKey(blk.records.back());
            if (extractKey(it->first) != newMaxKey) {
                index.erase(it);
                index[newMaxKey] = rbn;
            }
            cout << "Deleted zip " << zip << " from RBN " << rbn << endl;
        }

        deleted.insert(zip);
        --hb.header.recordCount;
    }

    data.close();
    ofstream outFile(blockedFile, ios::out | ios::binary);
    if (!outFile.is_open()) {
        cerr << "Failed to reopen blocked file for writing header." << endl;
        return 1;
    }
    hb.write(outFile);
    writeIndex(indexFile, index);

    cout << "Deleted " << deleted.size() << " records." << endl;
    return 0;
}