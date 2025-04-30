/// @file BPlusTree.h
/// @brief Header file for B+Tree implementation with file-based storage
/// @details Implements a B+Tree structure with both index and sequence blocks in a single file

#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include "CSVBuffer.h"

using namespace std;

/// Constants for B+Tree implementation
const int BLOCK_TYPE_INDEX = 0;
const int BLOCK_TYPE_SEQUENCE = 1;
const int INVALID_BLOCK = -1;

/// @brief Header structure for the B+Tree file
struct BTreeHeader {
    int version;           // Version identifier
    int blockSize;         // Size of each block in bytes
    int rootBlockNum;      // RBN of the root block
    int firstLeafBlockNum; // RBN of the first leaf in sequence set
    int lastLeafBlockNum;  // RBN of the last leaf in sequence set
    int totalBlocks;       // Total number of blocks in the file
    int order;             // Order of the B+Tree (max descendants per node)
    int recordsPerBlock;   // Maximum records per sequence block
    int keySize;           // Size of the key field in bytes
    
    /// @brief Default constructor with initialization
    BTreeHeader() 
        : version(1), blockSize(4096), rootBlockNum(1), firstLeafBlockNum(INVALID_BLOCK),
          lastLeafBlockNum(INVALID_BLOCK), totalBlocks(0), order(0), 
          recordsPerBlock(0), keySize(sizeof(int)) {}
};

/// @brief Base block structure - common elements for both block types
struct Block {
    int blockType;         // 0 = Index Block, 1 = Sequence Block
    int blockNum;          // Relative Block Number (RBN)
    int parentBlockNum;    // RBN of parent block (-1 if root)
    
    /// @brief Default constructor
    Block() : blockType(-1), blockNum(-1), parentBlockNum(INVALID_BLOCK) {}
    
    /// @brief Virtual destructor for polymorphism
    virtual ~Block() {}
    
    /// @brief Virtual method to pack block data into a byte array
    /// @param buffer Pointer to the buffer
    /// @return Number of bytes written
    virtual int pack(char* buffer) const = 0;
    
    /// @brief Virtual method to unpack block data from a byte array
    /// @param buffer Pointer to the buffer
    /// @return Number of bytes read
    virtual int unpack(const char* buffer) = 0;
};

/// @brief Key-Pointer pair for index blocks
struct KeyPointerPair {
    int key;               // Key value (largest in child block)
    int childBlockNum;     // RBN of child block
    
    /// @brief Default constructor
    KeyPointerPair() : key(0), childBlockNum(INVALID_BLOCK) {}
    
    /// @brief Constructor with values
    KeyPointerPair(int k, int block) : key(k), childBlockNum(block) {}
    
    /// @brief Less than operator for sorting
    bool operator<(const KeyPointerPair& other) const {
        return key < other.key;
    }
};

/// @brief Index block structure
struct IndexBlock : public Block {
    vector<KeyPointerPair> entries;  // Key-pointer pairs
    
    /// @brief Default constructor
    IndexBlock() {
        blockType = BLOCK_TYPE_INDEX;
    }
    
    /// @brief Pack index block data into a byte array
    /// @param buffer Pointer to the buffer
    /// @return Number of bytes written
    int pack(char* buffer) const override;
    
    /// @brief Unpack index block data from a byte array
    /// @param buffer Pointer to the buffer
    /// @return Number of bytes read
    int unpack(const char* buffer) override;
    
    /// @brief Find the child block that should contain the given key
    /// @param key The search key
    /// @return Index of the child pointer
    int findChildIndex(int key) const;
};

/// @brief Sequence block structure (leaf level with actual records)
struct SequenceBlock : public Block {
    int prevBlockNum;      // RBN of previous sequence block (-1 if first)
    int nextBlockNum;      // RBN of next sequence block (-1 if last)
    vector<ZipRecord> records;  // Data records
    
    /// @brief Default constructor
    SequenceBlock() : prevBlockNum(INVALID_BLOCK), nextBlockNum(INVALID_BLOCK) {
        blockType = BLOCK_TYPE_SEQUENCE;
    }
    
    /// @brief Pack sequence block data into a byte array
    /// @param buffer Pointer to the buffer
    /// @return Number of bytes written
    int pack(char* buffer) const override;
    
    /// @brief Unpack sequence block data from a byte array
    /// @param buffer Pointer to the buffer
    /// @return Number of bytes read
    int unpack(const char* buffer) override;
    
    /// @brief Find the record with the given ZIP code
    /// @param zipCode The ZIP code to search for
    /// @return Index of the record, or -1 if not found
    int findRecord(int zipCode) const;
    
    /// @brief Insert a record in sorted order
    /// @param record The record to insert
    /// @return True if successful, false if block is full
    bool insertRecord(const ZipRecord& record);
};

/// @brief Class for managing a B+Tree stored in a file
class BPlusTree {
private:
    string filename;           // Name of the B+Tree file
    fstream file;              // File stream for I/O
    BTreeHeader header;        // Tree header (kept in memory)
    
    /// @brief Read a block from the file
    /// @param blockNum The block number to read
    /// @param block Reference to the block object
    /// @return True if successful
    bool readBlock(int blockNum, Block& block);
    
    /// @brief Write a block to the file
    /// @param block The block to write
    /// @return True if successful
    bool writeBlock(const Block& block);
    
    /// @brief Allocate a new block in the file
    /// @return Block number of the new block
    int allocateBlock();
    
    /// @brief Find the leaf block that should contain the given key
    /// @param key The search key
    /// @return Block number of the leaf
    int findLeafBlock(int key);
    
    /// @brief Split an index block that is too full
    /// @param block The block to split
    /// @return New parent key and block
    KeyPointerPair splitIndexBlock(IndexBlock& block);
    
    /// @brief Split a sequence block that is too full
    /// @param block The block to split
    /// @return New parent key and block
    KeyPointerPair splitSequenceBlock(SequenceBlock& block);
    
    /// @brief Insert a new key-pointer pair into an index block
    /// @param blockNum The block number
    /// @param key The key to insert
    /// @param childBlockNum The child block pointer
    /// @return Block number of the new root if split occurred, 0 otherwise
    int insertIntoIndex(int blockNum, int key, int childBlockNum);
    
public:
    /// @brief Constructor that creates or opens a B+Tree file
    /// @param fname The name of the file
    /// @param blockSize The size of each block in bytes (for new files)
    /// @param createNew If true, create a new file; otherwise open existing
    BPlusTree(const string& fname, int blockSize = 4096, bool createNew = false);
    
    /// @brief Destructor
    ~BPlusTree();
    
    /// @brief Initialize a new B+Tree
    /// @param order The order of the tree (max children per node)
    /// @param recordsPerBlock Maximum records per sequence block
    void initialize(int order, int recordsPerBlock);
    
    /// @brief Bulk load records from a CSV file
    /// @param csvFilename The name of the CSV file
    void bulkLoad(const string& csvFilename);
    
    /// @brief Search for a record by ZIP code
    /// @param zipCode The ZIP code to search for
    /// @return The found record, or empty record if not found
    ZipRecord search(int zipCode);
    
    /// @brief Insert a new record
    /// @param record The record to insert
    /// @return True if successful
    bool insert(const ZipRecord& record);
    
    /// @brief Remove a record by ZIP code
    /// @param zipCode The ZIP code to remove
    /// @return True if the record was found and removed
    bool remove(int zipCode);
    
    /// @brief Print the B+Tree structure for debugging
    void printTree();
    
    /// @brief Get the total number of blocks in the file
    /// @return Number of blocks
    int getBlockCount() const { return header.totalBlocks; }
    
    /// @brief Get the height of the tree
    /// @return Tree height
    int getTreeHeight();
    
    /// @brief Traverse records in order
    /// @param callback Function to call for each record
    void traverseInOrder(void (*callback)(const ZipRecord&));
};

#endif // BPLUSTREE_H