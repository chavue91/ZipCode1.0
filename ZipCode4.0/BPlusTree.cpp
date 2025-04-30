/// @file BPlusTree.cpp
/// @brief Implementation of B+Tree with file-based storage
/// @details Implements a B+Tree structure with both index and sequence blocks in a single file

#include "BPlusTree.h"
#include <algorithm>
#include <queue>
#include <iomanip>
#include <cmath>
#include <climits>
#include <limits>

using namespace std;

//======================= IndexBlock implementation =======================

int IndexBlock::pack(char* buffer) const {
    int offset = 0;
    
    // Write block type
    memcpy(buffer + offset, &blockType, sizeof(int));
    offset += sizeof(int);
    
    // Write block number
    memcpy(buffer + offset, &blockNum, sizeof(int));
    offset += sizeof(int);
    
    // Write parent block number
    memcpy(buffer + offset, &parentBlockNum, sizeof(int));
    offset += sizeof(int);
    
    // Write number of entries
    int numEntries = entries.size();
    memcpy(buffer + offset, &numEntries, sizeof(int));
    offset += sizeof(int);
    
    // Write all key-pointer pairs
    for (const auto& entry : entries) {
        memcpy(buffer + offset, &entry.key, sizeof(int));
        offset += sizeof(int);
        
        memcpy(buffer + offset, &entry.childBlockNum, sizeof(int));
        offset += sizeof(int);
    }
    
    return offset;
}

int IndexBlock::unpack(const char* buffer) {
    int offset = 0;
    
    // Read block type
    memcpy(&blockType, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read block number
    memcpy(&blockNum, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read parent block number
    memcpy(&parentBlockNum, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read number of entries
    int numEntries;
    memcpy(&numEntries, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read all key-pointer pairs
    entries.clear();
    for (int i = 0; i < numEntries; i++) {
        KeyPointerPair entry;
        
        memcpy(&entry.key, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        memcpy(&entry.childBlockNum, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        entries.push_back(entry);
    }
    
    return offset;
}

int IndexBlock::findChildIndex(int key) const {
    // Find the index of the first entry whose key is greater than the search key
    for (size_t i = 0; i < entries.size(); i++) {
        if (key <= entries[i].key) {
            return i;
        }
    }
    
    // If the key is greater than all entries, return the last child
    return entries.size() - 1;
}

//======================= SequenceBlock implementation =======================

int SequenceBlock::pack(char* buffer) const {
    int offset = 0;
    
    // Write block type
    memcpy(buffer + offset, &blockType, sizeof(int));
    offset += sizeof(int);
    
    // Write block number
    memcpy(buffer + offset, &blockNum, sizeof(int));
    offset += sizeof(int);
    
    // Write parent block number
    memcpy(buffer + offset, &parentBlockNum, sizeof(int));
    offset += sizeof(int);
    
    // Write previous block number
    memcpy(buffer + offset, &prevBlockNum, sizeof(int));
    offset += sizeof(int);
    
    // Write next block number
    memcpy(buffer + offset, &nextBlockNum, sizeof(int));
    offset += sizeof(int);
    
    // Write number of records
    int numRecords = records.size();
    memcpy(buffer + offset, &numRecords, sizeof(int));
    offset += sizeof(int);
    
    // Write all records
    for (const auto& record : records) {
        // Pack ZIP code
        memcpy(buffer + offset, &record.zipCode, sizeof(int));
        offset += sizeof(int);
        
        // Pack place name (fixed length for simplicity)
        int placeNameLen = record.placeName.size();
        memcpy(buffer + offset, &placeNameLen, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, record.placeName.c_str(), placeNameLen);
        offset += placeNameLen;
        
        // Pack state (2-char code)
        int stateLen = record.state.size();
        memcpy(buffer + offset, &stateLen, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, record.state.c_str(), stateLen);
        offset += stateLen;
        
        // Pack county
        int countyLen = record.county.size();
        memcpy(buffer + offset, &countyLen, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, record.county.c_str(), countyLen);
        offset += countyLen;
        
        // Pack latitude and longitude
        memcpy(buffer + offset, &record.latitude, sizeof(double));
        offset += sizeof(double);
        
        memcpy(buffer + offset, &record.longitude, sizeof(double));
        offset += sizeof(double);
    }
    
    return offset;
}

int SequenceBlock::unpack(const char* buffer) {
    int offset = 0;
    
    // Read block type
    memcpy(&blockType, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read block number
    memcpy(&blockNum, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read parent block number
    memcpy(&parentBlockNum, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read previous block number
    memcpy(&prevBlockNum, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read next block number
    memcpy(&nextBlockNum, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read number of records
    int numRecords;
    memcpy(&numRecords, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Read all records
    records.clear();
    for (int i = 0; i < numRecords; i++) {
        ZipRecord record;
        
        // Unpack ZIP code
        memcpy(&record.zipCode, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        // Unpack place name
        int placeNameLen;
        memcpy(&placeNameLen, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        char* placeNameBuffer = new char[placeNameLen + 1];
        memcpy(placeNameBuffer, buffer + offset, placeNameLen);
        placeNameBuffer[placeNameLen] = '\0';
        record.placeName = string(placeNameBuffer);
        delete[] placeNameBuffer;
        offset += placeNameLen;
        
        // Unpack state
        int stateLen;
        memcpy(&stateLen, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        char* stateBuffer = new char[stateLen + 1];
        memcpy(stateBuffer, buffer + offset, stateLen);
        stateBuffer[stateLen] = '\0';
        record.state = string(stateBuffer);
        delete[] stateBuffer;
        offset += stateLen;
        
        // Unpack county
        int countyLen;
        memcpy(&countyLen, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        char* countyBuffer = new char[countyLen + 1];
        memcpy(countyBuffer, buffer + offset, countyLen);
        countyBuffer[countyLen] = '\0';
        record.county = string(countyBuffer);
        delete[] countyBuffer;
        offset += countyLen;
        
        // Unpack latitude and longitude
        memcpy(&record.latitude, buffer + offset, sizeof(double));
        offset += sizeof(double);
        
        memcpy(&record.longitude, buffer + offset, sizeof(double));
        offset += sizeof(double);
        
        records.push_back(record);
    }
    
    return offset;
}

int SequenceBlock::findRecord(int zipCode) const {
    for (size_t i = 0; i < records.size(); i++) {
        if (records[i].zipCode == zipCode) {
            return i;
        }
    }
    return -1;
}

bool SequenceBlock::insertRecord(const ZipRecord& record) {
    // Find the position to insert the record (maintain sorted order)
    auto it = lower_bound(records.begin(), records.end(), record, 
                         [](const ZipRecord& a, const ZipRecord& b) {
                             return a.zipCode < b.zipCode;
                         });
    
    // Insert the record at the correct position
    records.insert(it, record);
    return true;
}

//======================= BPlusTree implementation =======================

BPlusTree::BPlusTree(const string& fname, int blockSize, bool createNew) 
    : filename(fname) {
    
    if (createNew) {
        // Create a new file
        file.open(filename, ios::binary | ios::in | ios::out | ios::trunc);
        if (!file) {
            throw runtime_error("Failed to create B+Tree file: " + filename);
        }
        
        // Initialize header
        header = BTreeHeader();
        header.blockSize = blockSize;
        header.totalBlocks = 1;  // Just the header block initially
        
        // Write the header to the file
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char*>(&header), sizeof(BTreeHeader));
    } else {
        // Open existing file
        file.open(filename, ios::binary | ios::in | ios::out);
        if (!file) {
            throw runtime_error("Failed to open B+Tree file: " + filename);
        }
        
        // Read the header from the file
        file.seekg(0, ios::beg);
        file.read(reinterpret_cast<char*>(&header), sizeof(BTreeHeader));
        if (file.gcount() != sizeof(BTreeHeader)) {
            throw runtime_error("Failed to read B+Tree header");
        }
    }
}

BPlusTree::~BPlusTree() {
    if (file.is_open()) {
        // Write the header before closing
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char*>(&header), sizeof(BTreeHeader));
        file.close();
    }
}

void BPlusTree::initialize(int order, int recordsPerBlock) {
    // Set the B+Tree parameters
    header.order = order;
    header.recordsPerBlock = recordsPerBlock;
    
    // Create the root index block
    IndexBlock rootBlock;
    rootBlock.blockNum = 1;
    rootBlock.parentBlockNum = INVALID_BLOCK;  // Root has no parent
    
    // Create the first sequence block
    SequenceBlock leafBlock;
    leafBlock.blockNum = 2;
    leafBlock.parentBlockNum = 1;  // Parent is the root
    leafBlock.prevBlockNum = INVALID_BLOCK;
    leafBlock.nextBlockNum = INVALID_BLOCK;
    
    // Update header
    header.rootBlockNum = 1;
    header.firstLeafBlockNum = 2;
    header.lastLeafBlockNum = 2;
    header.totalBlocks = 3;  // Header + root + first leaf
    
    // Link the root to the leaf
    KeyPointerPair pair;
    pair.key = INT_MAX;  // Maximum possible key value
    pair.childBlockNum = 2;
    rootBlock.entries.push_back(pair);
    
    // Write blocks to file
    writeBlock(rootBlock);
    writeBlock(leafBlock);
    
    // Update the header
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<const char*>(&header), sizeof(BTreeHeader));
}

bool BPlusTree::readBlock(int blockNum, Block& block) {
    // Calculate the offset in the file
    size_t offset = sizeof(BTreeHeader) + (blockNum - 1) * header.blockSize;
    
    // Allocate a buffer for the block
    char* buffer = new char[header.blockSize];
    
    // Read the block from the file
    file.seekg(offset, ios::beg);
    file.read(buffer, header.blockSize);
    if (file.gcount() != header.blockSize) {
        delete[] buffer;
        return false;
    }
    
    // Determine the block type and unpack accordingly
    int blockType;
    memcpy(&blockType, buffer, sizeof(int));
    
    if (blockType == BLOCK_TYPE_INDEX) {
        // Cast to IndexBlock and unpack
        IndexBlock* indexBlock = dynamic_cast<IndexBlock*>(&block);
        if (!indexBlock) {
            indexBlock = new IndexBlock();
            indexBlock->unpack(buffer);
            // Copy relevant fields to the base Block
            block.blockType = indexBlock->blockType;
            block.blockNum = indexBlock->blockNum;
            block.parentBlockNum = indexBlock->parentBlockNum;
            delete indexBlock;
        } else {
            indexBlock->unpack(buffer);
        }
    } else if (blockType == BLOCK_TYPE_SEQUENCE) {
        // Cast to SequenceBlock and unpack
        SequenceBlock* seqBlock = dynamic_cast<SequenceBlock*>(&block);
        if (!seqBlock) {
            seqBlock = new SequenceBlock();
            seqBlock->unpack(buffer);
            // Copy relevant fields to the base Block
            block.blockType = seqBlock->blockType;
            block.blockNum = seqBlock->blockNum;
            block.parentBlockNum = seqBlock->parentBlockNum;
            delete seqBlock;
        } else {
            seqBlock->unpack(buffer);
        }
    } else {
        delete[] buffer;
        return false;
    }
    
    delete[] buffer;
    return true;
}

bool BPlusTree::writeBlock(const Block& block) {
    // Calculate the offset in the file
    size_t offset = sizeof(BTreeHeader) + (block.blockNum - 1) * header.blockSize;
    
    // Allocate a buffer for the block
    char* buffer = new char[header.blockSize];
    memset(buffer, 0, header.blockSize);  // Initialize with zeros
    
    // Pack the block into the buffer
    int bytesWritten = block.pack(buffer);
    
    // Write the buffer to the file
    file.seekp(offset, ios::beg);
    file.write(buffer, header.blockSize);
    
    delete[] buffer;
    return file.good();
}

int BPlusTree::allocateBlock() {
    // Get the next available block number
    int blockNum = header.totalBlocks + 1;
    
    // Increase the total block count
    header.totalBlocks = blockNum;
    
    // Return the new block number
    return blockNum;
}

void BPlusTree::bulkLoad(const string& csvFilename) {
    // Load records from the CSV file
    CSVBuffer csvBuffer(csvFilename);
    const vector<ZipRecord>& records = csvBuffer.getRecords();
    
    // Sort records by ZIP code
    vector<ZipRecord> sortedRecords = records;
    sort(sortedRecords.begin(), sortedRecords.end(), 
         [](const ZipRecord& a, const ZipRecord& b) {
             return a.zipCode < b.zipCode;
         });
    
    // Clear existing B+Tree if any
    initialize(header.order, header.recordsPerBlock);
    
    // Create and fill leaf blocks
    vector<int> leafBlocks;
    vector<int> leafMaxKeys;
    int recordsPerLeaf = header.recordsPerBlock;
    
    for (size_t i = 0; i < sortedRecords.size(); i += recordsPerLeaf) {
        // Create a new leaf block
        SequenceBlock leafBlock;
        leafBlock.blockNum = allocateBlock();
        leafBlock.parentBlockNum = INVALID_BLOCK;  // Will set this later
        
        // Set previous and next pointers
        if (!leafBlocks.empty()) {
            leafBlock.prevBlockNum = leafBlocks.back();
            
            // Update the next pointer of the previous leaf
            SequenceBlock prevLeaf;
            readBlock(leafBlock.prevBlockNum, prevLeaf);
            prevLeaf.nextBlockNum = leafBlock.blockNum;
            writeBlock(prevLeaf);
        } else {
            leafBlock.prevBlockNum = INVALID_BLOCK;
            header.firstLeafBlockNum = leafBlock.blockNum;
        }
        
        // Set the leaf as the last leaf in the chain
        leafBlock.nextBlockNum = INVALID_BLOCK;
        header.lastLeafBlockNum = leafBlock.blockNum;
        
        // Add records to the leaf
        size_t end = min(i + recordsPerLeaf, sortedRecords.size());
        for (size_t j = i; j < end; j++) {
            leafBlock.records.push_back(sortedRecords[j]);
        }
        
        // Remember the maximum key in this leaf
        if (!leafBlock.records.empty()) {
            leafMaxKeys.push_back(leafBlock.records.back().zipCode);
        } else {
            leafMaxKeys.push_back(INT_MAX);
        }
        
        // Write the leaf block to the file
        writeBlock(leafBlock);
        
        // Keep track of the leaf blocks
        leafBlocks.push_back(leafBlock.blockNum);
    }
    
    // Build the index nodes from bottom up
    vector<int> currentLevel = leafBlocks;
    vector<int> maxKeys = leafMaxKeys;
    
    while (currentLevel.size() > 1) {
        vector<int> nextLevel;
        vector<int> nextMaxKeys;
        
        for (size_t i = 0; i < currentLevel.size(); i += header.order) {
            // Create a new index block
            IndexBlock indexBlock;
            indexBlock.blockNum = allocateBlock();
            indexBlock.parentBlockNum = INVALID_BLOCK;  // Will set this later
            
            // Add entries to the index block
            size_t end = min(i + header.order, currentLevel.size());
            for (size_t j = i; j < end; j++) {
                KeyPointerPair pair;
                pair.key = maxKeys[j];
                pair.childBlockNum = currentLevel[j];
                indexBlock.entries.push_back(pair);
                
                // Update the parent of the child block
                if (j < i + header.order - 1) {
                    // Determine the type of the child block
                    int blockType;
                    file.seekg(sizeof(BTreeHeader) + (pair.childBlockNum - 1) * header.blockSize, ios::beg);
                    file.read(reinterpret_cast<char*>(&blockType), sizeof(int));
                    
                    if (blockType == BLOCK_TYPE_INDEX) {
                        IndexBlock childBlock;
                        readBlock(pair.childBlockNum, childBlock);
                        childBlock.parentBlockNum = indexBlock.blockNum;
                        writeBlock(childBlock);
                    } else {
                        SequenceBlock childBlock;
                        readBlock(pair.childBlockNum, childBlock);
                        childBlock.parentBlockNum = indexBlock.blockNum;
                        writeBlock(childBlock);
                    }
                }
            }
            
            // Write the index block to the file
            writeBlock(indexBlock);
            
            // Keep track of the index blocks for the next level
            nextLevel.push_back(indexBlock.blockNum);
            
            // Keep track of the maximum key in this index block
            if (!indexBlock.entries.empty()) {
                nextMaxKeys.push_back(indexBlock.entries.back().key);
            } else {
                nextMaxKeys.push_back(INT_MAX);
            }
        }
        
        // Move up to the next level
        currentLevel = nextLevel;
        maxKeys = nextMaxKeys;
    }
    
    // Set the root block
    header.rootBlockNum = currentLevel[0];
    
    // Update the header
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<const char*>(&header), sizeof(BTreeHeader));
}

int BPlusTree::findLeafBlock(int key) {
    int currentBlockNum = header.rootBlockNum;
    
    while (true) {
        // Read the current block
        int blockType;
        file.seekg(sizeof(BTreeHeader) + (currentBlockNum - 1) * header.blockSize, ios::beg);
        file.read(reinterpret_cast<char*>(&blockType), sizeof(int));
        
        if (blockType == BLOCK_TYPE_SEQUENCE) {
            // We've reached a leaf block
            return currentBlockNum;
        } else {
            // Navigate down the tree
            IndexBlock indexBlock;
            readBlock(currentBlockNum, indexBlock);
            
            // Find the child pointer
            int childIndex = 0;
            for (size_t i = 0; i < indexBlock.entries.size(); i++) {
                if (key <= indexBlock.entries[i].key) {
                    childIndex = i;
                    break;
                }
                if (i == indexBlock.entries.size() - 1) {
                    childIndex = i;
                }
            }
            
            currentBlockNum = indexBlock.entries[childIndex].childBlockNum;
        }
    }
}

ZipRecord BPlusTree::search(int zipCode) {
    // Find the leaf block that should contain the record
    int leafBlockNum = findLeafBlock(zipCode);
    
    // Read the leaf block
    SequenceBlock leafBlock;
    readBlock(leafBlockNum, leafBlock);
    
    // Search for the record in the leaf block
    for (const auto& record : leafBlock.records) {
        if (record.zipCode == zipCode) {
            return record;
        }
    }
    
    // Record not found
    return ZipRecord();
}

bool BPlusTree::insert(const ZipRecord& record) {
    // Find the leaf block where the record should be inserted
    int leafBlockNum = findLeafBlock(record.zipCode);
    
    // Read the leaf block
    SequenceBlock leafBlock;
    readBlock(leafBlockNum, leafBlock);
    
    // Check if the record already exists
    for (size_t i = 0; i < leafBlock.records.size(); i++) {
        if (leafBlock.records[i].zipCode == record.zipCode) {
            // Update the existing record
            leafBlock.records[i] = record;
            writeBlock(leafBlock);
            return true;
        }
    }
    
    // Insert the new record
    leafBlock.insertRecord(record);
    
    // Check if the leaf block needs to be split
    if (leafBlock.records.size() <= header.recordsPerBlock) {
        // No split needed, just write the updated block
        writeBlock(leafBlock);
        return true;
    } else {
        // Split the leaf block
        KeyPointerPair newEntry = splitSequenceBlock(leafBlock);
        
        // Insert the new entry into the parent
        return insertIntoIndex(leafBlock.parentBlockNum, newEntry.key, newEntry.childBlockNum) == 0;
    }
}

KeyPointerPair BPlusTree::splitSequenceBlock(SequenceBlock& block) {
    // Create a new sequence block
    SequenceBlock newBlock;
    newBlock.blockNum = allocateBlock();
    newBlock.parentBlockNum = block.parentBlockNum;
    
    // Set up the linked list pointers
    newBlock.nextBlockNum = block.nextBlockNum;
    newBlock.prevBlockNum = block.blockNum;
    
    if (block.nextBlockNum != INVALID_BLOCK) {
        // Update the previous pointer of the next block
        SequenceBlock nextBlock;
        readBlock(block.nextBlockNum, nextBlock);
        nextBlock.prevBlockNum = newBlock.blockNum;
        writeBlock(nextBlock);
    } else {
        // This was the last block, so update the header
        header.lastLeafBlockNum = newBlock.blockNum;
    }
    
    // Update the next pointer of the original block
    block.nextBlockNum = newBlock.blockNum;
    
    // Distribute records between the blocks
    int midpoint = block.records.size() / 2;
    newBlock.records.assign(block.records.begin() + midpoint, block.records.end());
    block.records.resize(midpoint);
    
    // Write both blocks to the file
    writeBlock(block);
    writeBlock(newBlock);
    
    // Create a new entry for the parent
    KeyPointerPair newEntry;
    newEntry.key = newBlock.records.front().zipCode;
    newEntry.childBlockNum = newBlock.blockNum;
    
    return newEntry;
}

int BPlusTree::insertIntoIndex(int blockNum, int key, int childBlockNum) {
    // If the block is the root, and there's no root yet, create one
    if (blockNum == INVALID_BLOCK) {
        // Create a new root block
        IndexBlock rootBlock;
        rootBlock.blockNum = allocateBlock();
        rootBlock.parentBlockNum = INVALID_BLOCK;
        
        // Add the entries
        KeyPointerPair entry1;
        entry1.key = key - 1;  // Ensure the first range includes everything up to the split point
        entry1.childBlockNum = header.firstLeafBlockNum;
        rootBlock.entries.push_back(entry1);
        
        KeyPointerPair entry2;
        entry2.key = key;
        entry2.childBlockNum = childBlockNum;
        rootBlock.entries.push_back(entry2);
        
        // Update the parent pointers of the children
        SequenceBlock childBlock1, childBlock2;
        readBlock(entry1.childBlockNum, childBlock1);
        readBlock(entry2.childBlockNum, childBlock2);
        
        childBlock1.parentBlockNum = rootBlock.blockNum;
        childBlock2.parentBlockNum = rootBlock.blockNum;
        
        writeBlock(childBlock1);
        writeBlock(childBlock2);
        
        // Update the root in the header
        header.rootBlockNum = rootBlock.blockNum;
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char*>(&header), sizeof(BTreeHeader));
        
        // Write the new root block
        writeBlock(rootBlock);
        
        return rootBlock.blockNum;
    }
    
    // Read the index block
    IndexBlock indexBlock;
    readBlock(blockNum, indexBlock);
    
    // Find the position to insert the new entry
    auto it = lower_bound(indexBlock.entries.begin(), indexBlock.entries.end(), 
                         KeyPointerPair(key, 0), 
                         [](const KeyPointerPair& a, const KeyPointerPair& b) {
                             return a.key < b.key;
                         });
    
    // Insert the new entry
    KeyPointerPair newEntry;
    newEntry.key = key;
    newEntry.childBlockNum = childBlockNum;
    indexBlock.entries.insert(it, newEntry);
    
    // Check if the index block needs to be split
    if (indexBlock.entries.size() <= header.order) {
        // No split needed, just write the updated block
        writeBlock(indexBlock);
        return 0;
    } else {
        // Split the index block
        KeyPointerPair newParentEntry = splitIndexBlock(indexBlock);
        
        // If this is the root, create a new root
        if (indexBlock.parentBlockNum == INVALID_BLOCK) {
            // Create a new root block
            IndexBlock newRootBlock;
            newRootBlock.blockNum = allocateBlock();
            newRootBlock.parentBlockNum = INVALID_BLOCK;
            
            // Add the entries to the new root
            KeyPointerPair entry1;
            entry1.key = indexBlock.entries.back().key;
            entry1.childBlockNum = indexBlock.blockNum;
            newRootBlock.entries.push_back(entry1);
            
            KeyPointerPair entry2;
            entry2.key = newParentEntry.key;
            entry2.childBlockNum = newParentEntry.childBlockNum;
            newRootBlock.entries.push_back(entry2);
            
            // Update the parent pointers of the children
            indexBlock.parentBlockNum = newRootBlock.blockNum;
            writeBlock(indexBlock);
            
            IndexBlock newChildBlock;
            readBlock(newParentEntry.childBlockNum, newChildBlock);
            newChildBlock.parentBlockNum = newRootBlock.blockNum;
            writeBlock(newChildBlock);
            
            // Update the root in the header
            header.rootBlockNum = newRootBlock.blockNum;
            file.seekp(0, ios::beg);
            file.write(reinterpret_cast<const char*>(&header), sizeof(BTreeHeader));
            
            // Write the new root block
            writeBlock(newRootBlock);
            
            return newRootBlock.blockNum;
        } else {
            // Insert the new entry into the parent
            return insertIntoIndex(indexBlock.parentBlockNum, newParentEntry.key, newParentEntry.childBlockNum);
        }
    }
}

KeyPointerPair BPlusTree::splitIndexBlock(IndexBlock& block) {
    // Create a new index block
    IndexBlock newBlock;
    newBlock.blockNum = allocateBlock();
    newBlock.parentBlockNum = block.parentBlockNum;
    
    // Distribute entries between the blocks
    int midpoint = block.entries.size() / 2;
    
    // Move entries from old block to new block
    newBlock.entries.assign(block.entries.begin() + midpoint + 1, block.entries.end());
    
    // The key to be promoted to the parent
    KeyPointerPair promoted = block.entries[midpoint];
    
    // Shrink the original block
    block.entries.resize(midpoint);
    
    // Update the parent pointers of the children in the new block
    for (const auto& entry : newBlock.entries) {
        // Determine the type of the child block
        int blockType;
        file.seekg(sizeof(BTreeHeader) + (entry.childBlockNum - 1) * header.blockSize, ios::beg);
        file.read(reinterpret_cast<char*>(&blockType), sizeof(int));
        
        if (blockType == BLOCK_TYPE_INDEX) {
            IndexBlock childBlock;
            readBlock(entry.childBlockNum, childBlock);
            childBlock.parentBlockNum = newBlock.blockNum;
            writeBlock(childBlock);
        } else {
            SequenceBlock childBlock;
            readBlock(entry.childBlockNum, childBlock);
            childBlock.parentBlockNum = newBlock.blockNum;
            writeBlock(childBlock);
        }
    }
    
    // Write both blocks to the file
    writeBlock(block);
    writeBlock(newBlock);
    
    // Create a new entry for the parent
    KeyPointerPair newEntry;
    newEntry.key = promoted.key;
    newEntry.childBlockNum = newBlock.blockNum;
    
    return newEntry;
}

bool BPlusTree::remove(int zipCode) {
    // Find the leaf block that should contain the record
    int leafBlockNum = findLeafBlock(zipCode);
    
    // Read the leaf block
    SequenceBlock leafBlock;
    readBlock(leafBlockNum, leafBlock);
    
    // Find the record in the leaf block
    int recordIndex = leafBlock.findRecord(zipCode);
    if (recordIndex == -1) {
        // Record not found
        return false;
    }
    
    // Remove the record
    leafBlock.records.erase(leafBlock.records.begin() + recordIndex);
    
    // Write the updated block
    writeBlock(leafBlock);
    
    // TODO: Handle underflow (merge or redistribute if block is too empty)
    // This would require a more complex implementation
    
    return true;
}

int BPlusTree::getTreeHeight() {
    int height = 1;  // Start with the root
    int currentBlockNum = header.rootBlockNum;
    
    while (true) {
        // Read the current block
        int blockType;
        file.seekg(sizeof(BTreeHeader) + (currentBlockNum - 1) * header.blockSize, ios::beg);
        file.read(reinterpret_cast<char*>(&blockType), sizeof(int));
        
        if (blockType == BLOCK_TYPE_SEQUENCE) {
            // We've reached a leaf block
            return height;
        } else {
            // Navigate down the tree to the first child
            IndexBlock indexBlock;
            readBlock(currentBlockNum, indexBlock);
            
            if (indexBlock.entries.empty()) {
                // Something is wrong
                return height;
            }
            
            currentBlockNum = indexBlock.entries[0].childBlockNum;
            height++;
        }
    }
}

void BPlusTree::printTree() {
    cout << "B+Tree Structure:" << endl;
    cout << "Total blocks: " << header.totalBlocks << endl;
    cout << "Root block: " << header.rootBlockNum << endl;
    cout << "First leaf: " << header.firstLeafBlockNum << endl;
    cout << "Last leaf: " << header.lastLeafBlockNum << endl;
    cout << "Tree height: " << getTreeHeight() << endl;
    cout << endl;
    
    // Print the tree structure level by level
    queue<int> blockQueue;
    blockQueue.push(header.rootBlockNum);
    
    int level = 0;
    int nodesInCurrentLevel = 1;
    int nodesInNextLevel = 0;
    
    cout << "Level " << level << ": ";
    
    while (!blockQueue.empty()) {
        int currentBlockNum = blockQueue.front();
        blockQueue.pop();
        nodesInCurrentLevel--;
        
        // Read the block type
        int blockType;
        file.seekg(sizeof(BTreeHeader) + (currentBlockNum - 1) * header.blockSize, ios::beg);
        file.read(reinterpret_cast<char*>(&blockType), sizeof(int));
        
        if (blockType == BLOCK_TYPE_INDEX) {
            // Index block
            IndexBlock indexBlock;
            readBlock(currentBlockNum, indexBlock);
            
            cout << "[" << currentBlockNum << ":I(" << indexBlock.entries.size() << ")] ";
            
            // Queue the children for the next level
            for (const auto& entry : indexBlock.entries) {
                blockQueue.push(entry.childBlockNum);
                nodesInNextLevel++;
            }
        } else {
            // Sequence block
            SequenceBlock seqBlock;
            readBlock(currentBlockNum, seqBlock);
            
            cout << "[" << currentBlockNum << ":S(" << seqBlock.records.size() << ")] ";
        }
        
        // Move to the next level if needed
        if (nodesInCurrentLevel == 0) {
            if (nodesInNextLevel > 0) {
                level++;
                nodesInCurrentLevel = nodesInNextLevel;
                nodesInNextLevel = 0;
                cout << endl << "Level " << level << ": ";
            }
        }
    }
    
    cout << endl;
    
    // Print all leaf blocks in sequence
    cout << "Sequence set (linked list of leaf blocks):" << endl;
    int currentLeaf = header.firstLeafBlockNum;
    
    while (currentLeaf != INVALID_BLOCK) {
        SequenceBlock leafBlock;
        readBlock(currentLeaf, leafBlock);
        
        cout << "Block " << currentLeaf << " (prev=" << leafBlock.prevBlockNum
             << ", next=" << leafBlock.nextBlockNum << "): ";
        
        for (const auto& record : leafBlock.records) {
            cout << record.zipCode << " ";
        }
        
        cout << endl;
        
        currentLeaf = leafBlock.nextBlockNum;
    }
}

void BPlusTree::traverseInOrder(void (*callback)(const ZipRecord&)) {
    // Start with the first leaf block
    int currentLeaf = header.firstLeafBlockNum;
    
    while (currentLeaf != INVALID_BLOCK) {
        SequenceBlock leafBlock;
        readBlock(currentLeaf, leafBlock);
        
        // Process all records in the leaf
        for (const auto& record : leafBlock.records) {
            callback(record);
        }
        
        // Move to the next leaf
        currentLeaf = leafBlock.nextBlockNum;
    }
}