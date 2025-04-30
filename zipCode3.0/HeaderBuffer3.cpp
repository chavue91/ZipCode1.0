/// @file HeaderBuffer3.cpp
/// @brief Implements methods for reading and writing blocked sequence set file headers.

#include "HeaderBuffer3.h"
#include <iostream>

using namespace std;

/**
 * @brief Reads a header record from the given input stream.
 * @param in The input stream to read the header from.
 * @return true if the header was successfully read, false otherwise.
 */
bool HeaderBuffer3::read(istream& in) {
    if (!in) return false;

    // Read fixed metadata fields line-by-line
    getline(in, header.fileStructureType);
    getline(in, header.version);
    in >> header.headerSize;
    in >> header.recordSizeFieldLength;
    in.ignore(); // skip newline
    getline(in, header.sizeFormatType);
    in >> header.blockSize;
    in >> header.minBlockCapacity;
    in.ignore();
    getline(in, header.indexFileName);
    getline(in, header.indexSchema);
    in >> header.recordCount;
    in >> header.blockCount;
    in >> header.fieldsPerRecord;
    in.ignore();

    // Read field definitions
    header.fields.clear();
    for (int i = 0; i < header.fieldsPerRecord; ++i) {
        Field f = readField(in);
        header.fields.push_back(f);
    }

    // Read final metadata for primary key and list heads
    in >> header.primaryKeyOrdinal;
    in >> header.availListHeadRBN;
    in >> header.sequenceListHeadRBN;
    in >> header.stale;
    in.ignore();

    return true;
}

/**
 * @brief Writes the header record to the given output stream.
 * @param out The output file stream to write the header to.
 */
void HeaderBuffer3::write(ostream& out) const {
    if (!out) return;

    // Write basic metadata fields
    out << header.fileStructureType << '\n';
    out << header.version << '\n';
    out << header.headerSize << '\n';
    out << header.recordSizeFieldLength << '\n';
    out << header.sizeFormatType << '\n';
    out << header.blockSize << '\n';
    out << header.minBlockCapacity << '\n';
    out << header.indexFileName << '\n';
    out << header.indexSchema << '\n';
    out << header.recordCount << '\n';
    out << header.blockCount << '\n';
    out << header.fieldsPerRecord << '\n';

    // Write field definitions
    for (const auto& field : header.fields) {
        writeField(out, field);
    }

    // Write list head metadata
    out << header.primaryKeyOrdinal << '\n';
    out << header.availListHeadRBN << '\n';
    out << header.sequenceListHeadRBN << '\n';
    out << header.stale << '\n';
}

/**
 * @brief Reads a single Field structure from the stream.
 * @param in The input stream to read from.
 * @return A Field populated with values read from the stream.
 */
Field HeaderBuffer3::readField(istream& in) {
    Field field;
    getline(in, field.name);
    getline(in, field.type);
    getline(in, field.format);
    in >> field.isPrimaryKey;
    in.ignore();
    return field;
}

/**
 * @brief Writes a single Field structure to the stream.
 * @param out The output stream.
 * @param field The Field to write.
 */
void HeaderBuffer3::writeField(ostream& out, const Field& field) const {
    out << field.name << '\n';
    out << field.type << '\n';
    out << field.format << '\n';
    out << field.isPrimaryKey << '\n';
}