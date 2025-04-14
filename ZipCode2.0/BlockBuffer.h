/// @file BlockBuffer.h
/// @brief Defines the BlockBuffer class for managing read/write operations on blocked sequence set blocks.

#ifndef BLOCKBUFFER_H
#define BLOCKBUFFER_H

#include "HeaderAndBlockStructures.h"
#include <fstream>

/// @brief Handles packing and unpacking of active and avail blocks in the blocked sequence set.
class BlockBuffer {
public:
    /// @brief Writes a block to the output stream at the specified RBN.
    /// @param out Output file stream (binary mode).
    /// @param block The block structure to write.
    /// @param rbn The relative block number (RBN) to write at.
    /// @param blockSize The fixed size of each block in bytes.
    /// @return True if write is successful.
    static bool writeBlock(std::fstream& out, const Block& block, int rbn, int blockSize);

    /// @brief Reads a block from the input stream at the specified RBN.
    /// @param in Input stream (binary mode).
    /// @param block Destination block structure to populate.
    /// @param rbn The relative block number (RBN) to read from.
    /// @param blockSize The fixed size of each block in bytes.
    /// @return True if read is successful.
    static bool readBlock(std::istream& in, Block& block, int rbn, int blockSize);

    /// @brief Writes an avail block to the output stream at the specified RBN.
    /// @param out Output file stream (binary mode).
    /// @param availBlock The avail block to write.
    /// @param rbn The RBN to write at.
    /// @param blockSize The fixed size of each block in bytes.
    /// @return True if write is successful.
    static void writeAvailBlock(std::ostream& out, const AvailBlock& avail, int rbn, int blockSize);

    /// @brief Reads the RBN of the next avail block (for traversing avail list).
    /// @param in Input file stream (binary mode).
    /// @param rbn The current avail block RBN.
    /// @param blockSize The fixed block size.
    /// @return RBN of next avail block or -1 on error.
    static int readNextAvailRBN(std::ifstream& in, int rbn, int blockSize);
};

#endif // BLOCKBUFFER_H