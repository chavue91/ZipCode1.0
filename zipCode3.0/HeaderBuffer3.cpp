/// @file HeaderBuffer3.cpp
/// @brief Implements methods for reading and writing blocked sequence set file headers.

#include "HeaderBuffer3.h"
#include <iostream>

using namespace std;

bool HeaderBuffer3::read(istream& in) {
    if (!in) return false;

    getline(in, header.fileStructureType);
    getline(in, header.version);
    in >> header.headerSize;
    in >> header.recordSizeFieldLength;
    in.ignore();
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

    header.fields.clear();
    for (int i = 0; i < header.fieldsPerRecord; ++i) {
        Field f = readField(in);
        header.fields.push_back(f);
    }

    in >> header.primaryKeyOrdinal;
    in >> header.availListHeadRBN;
    in >> header.sequenceListHeadRBN;
    in >> header.stale;
    in.ignore();

    return true;
}

void HeaderBuffer3::write(ofstream& out) {
    if (!out.is_open()) return;

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

    for (const auto& field : header.fields) {
        writeField(out, field);
    }

    out << header.primaryKeyOrdinal << '\n';
    out << header.availListHeadRBN << '\n';
    out << header.sequenceListHeadRBN << '\n';
    out << header.stale << '\n';
}

Field HeaderBuffer3::readField(istream& in) {
    Field field;
    getline(in, field.name);
    getline(in, field.type);
    getline(in, field.format);
    in >> field.isPrimaryKey;
    in.ignore();
    return field;
}

void HeaderBuffer3::writeField(ofstream& out, const Field& field) const {
    out << field.name << '\n';
    out << field.type << '\n';
    out << field.format << '\n';
    out << field.isPrimaryKey << '\n';
}
