/// @file InsertRecord.cpp
/// @brief Handles insertion of new ZIP code records into a blocked sequence set.

#include "HeaderBuffer3.h"
#include "BlockBuffer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

/// @brief Extracts the primary key (ZIP code) from a record string.
/// @param record A comma-separated string representing a ZIP code record.
/// @return The primary key (ZIP code).
string extractKey(const string& record) {
    stringstream ss(record);
    string key;
    getline(ss, key, ',');
    return key;
}

/// @brief Loads the index file into a map of <key, RBN> pairs.
/// @param indexFile Path to the index file.
/// @return Map containing highest keys and their corresponding RBNs.
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

/// @brief Writes the index map to the index file.
/// @param indexFile Path to the index file.
/// @param index Map containing updated <key, RBN> pairs.
void writeIndex(const string& indexFile, const map<string, int>& index) {
    ofstream out(indexFile);
    for (const auto& [key, rbn] : index) {
        out << key << "," << rbn << endl;
    }
}

/// @brief Main function for inserting ZIP code records into a blocked sequence set.
/// @param argc Argument count.
/// @param argv Command-line arguments: <blocked_file> <index_file> <insert_file>.
/// @return 0 on success, 1 on failure.
int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <blocked_file> <index_file> <insert_file>" << endl;
        return 1;
    }

    string blockedFile = argv[1];
    string indexFile = argv[2];
    string insertFile = argv[3];

    // Load index from file
    map<string, int> index = loadIndex(indexFile);
    if (index.empty()) {
        cerr << "Empty or missing index file." << endl;
        return 1;
    }

    // Open blocked sequence set file
    fstream data(blockedFile, ios::in | ios::out | ios::binary);
    if (!data.is_open()) {
        cerr << "Failed to open blocked file." << endl;
        return 1;
    }

    // Read header
    HeaderBuffer3 hb;
    if (!hb.read(data)) {
        cerr << "Failed to read header." << endl;
        return 1;
    }

    int blockSize = hb.header.blockSize;
    int nextRBN = hb.header.blockCount;  // Start at next unused block
    int inserted = 0;                    // Count of records inserted

    // Open file of new records to insert
    ifstream newRecords(insertFile);
    string newRec;
    while (getline(newRecords, newRec)) {
        string newKey = extractKey(newRec);
        auto it = index.lower_bound(newKey);
        int targetRBN = (it != index.end()) ? it->second : (--index.end())->second;

        // Load the target block and insert new record
        Block blk;
        BlockBuffer::readBlock(static_cast<istream&>(data), blk, targetRBN, blockSize);
        blk.records.push_back(newRec);

        // Keep block sorted
        sort(blk.records.begin(), blk.records.end(), [](const string& a, const string& b) {
            return extractKey(a) < extractKey(b);
        });

        // Estimate block size
        stringstream sizeSim;
        for (auto& r : blk.records) sizeSim << r << '\n';

        if (static_cast<int>(sizeSim.str().size()) < blockSize) {
            // Block fits, update it
            blk.recordCount = blk.records.size();
            BlockBuffer::writeBlock(data, blk, targetRBN, blockSize);
        } else {
            // Split the block into two
            vector<string> left(blk.records.begin(), blk.records.begin() + blk.records.size() / 2);
            vector<string> right(blk.records.begin() + blk.records.size() / 2, blk.records.end());

            Block leftBlock = { (int)left.size(), blk.prevRBN, nextRBN, left };
            Block rightBlock = { (int)right.size(), targetRBN, blk.nextRBN, right };

            BlockBuffer::writeBlock(data, leftBlock, targetRBN, blockSize);
            BlockBuffer::writeBlock(data, rightBlock, nextRBN, blockSize);

            // Update index with new keys
            string highLeft = extractKey(left.back());
            string highRight = extractKey(right.back());
            index.erase(highLeft);
            index[highLeft] = targetRBN;
            index[highRight] = nextRBN;

            ++nextRBN;
            ++hb.header.blockCount;
            cout << "Split block " << targetRBN << " → " << nextRBN - 1 << endl;
        }
        ++hb.header.recordCount;
        ++inserted;
    }

    // Rewind and update header in file
    data.seekp(0);
    std::ofstream outStream(blockedFile, ios::in | ios::out | ios::binary);
    hb.write(outStream);

    // Save updated index
    writeIndex(indexFile, index);

    cout << "Inserted " << inserted << " records." << endl;
    return 0;
}