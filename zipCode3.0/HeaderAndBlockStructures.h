/// @file HeaderAndBlockStructures.h
/// @brief Defines structures for the header and block layouts used in the Blocked Sequence Set.

#ifndef HEADER_AND_BLOCK_STRUCTURES_H
#define HEADER_AND_BLOCK_STRUCTURES_H

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

/// @brief Structure representing metadata for a single field in the record.
struct Field {
    string name;       ///< Field name or ID.
    string type;       ///< Field data type.
    string format;     ///< Format description (e.g. CSV, text).
    bool isPrimaryKey; ///< True if this field is the primary key.
};

/// @brief Structure representing the header of a blocked sequence set file.
struct Header {
    string fileStructureType; ///< e.g., "BlockedSequenceSet"
    string version;           ///< Version of the format.
    int headerSize;           ///< Total size in bytes of the header.
    int recordSizeFieldLength; ///< Size in bytes of the length field for each record.
    string sizeFormatType;    ///< e.g., "ASCII" or "binary"
    int blockSize;            ///< Size of each block in bytes (e.g., 512).
    float minBlockCapacity;   ///< Minimum block fill ratio (e.g., 0.5).
    string indexFileName;     ///< Associated index file.
    string indexSchema;       ///< Description of index format.
    int recordCount;          ///< Total number of records.
    int blockCount;           ///< Total number of blocks.
    int fieldsPerRecord;      ///< Number of fields in each record.
    vector<Field> fields;     ///< Field metadata list.
    int primaryKeyOrdinal;    ///< Index of primary key field in the fields vector.
    int availListHeadRBN;     ///< Head of the avail list (RBN).
    int sequenceListHeadRBN;  ///< Head of the active block list (RBN).
    bool stale;               ///< Indicates if the data is stale.
};

/// @brief Structure for an active block in the blocked sequence set.
struct Block {
    int recordCount;            ///< Number of valid records in this block.
    int prevRBN;                ///< Relative Block Number of the previous block.
    int nextRBN;                ///< Relative Block Number of the next block.
    vector<string> records;     ///< Raw length-indicated, comma-separated record strings.
};

/// @brief Structure for a free/available block.
struct AvailBlock {
    int recordCount = 0;        ///< Should always be 0 for an avail block.
    int nextAvailRBN;           ///< Pointer to the next available block in the avail list.
    string blankFill;           ///< Fill remaining block with whitespace for reuse.

    /// @brief Constructor to initialize and blank-fill an avail block.
    AvailBlock(int blockSize) {
        blankFill = string(blockSize - sizeof(int) * 2, ' ');
    }
};

#endif // HEADER_AND_BLOCK_STRUCTURES_H