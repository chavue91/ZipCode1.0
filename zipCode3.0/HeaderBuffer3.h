/// @file HeaderBuffer3.h
/// @brief Provides a class for reading and writing header records for blocked sequence set files.

#ifndef HEADERBUFFER3_H
#define HEADERBUFFER3_H

#include "HeaderAndBlockStructures.h"
#include <fstream>

using namespace std;

/// @brief Class responsible for reading/writing the Header to/from a file.
class HeaderBuffer3 {
public:
    Header header; ///< The header structure being managed.

    /// @brief Reads a header record from the input stream.
    /// @param in Reference to the input stream (e.g. ifstream or fstream).
    /// @return True if read successfully, false otherwise.
    bool read(istream& in);

    /// @brief Writes the header record to the output stream.
    /// @param out Reference to the output file stream.
    /// @return True if write successful, false otherwise.
    void write(ostream& out) const;

private:
    /// @brief Helper to read a single field definition from stream.
    Field readField(istream& in);

    /// @brief Helper to write a single field definition to stream.
    void writeField(ostream& out, const Field& field) const;
};

#endif // HEADERBUFFER3_H
