/// @file RecordBuffer.cpp
/// @brief Implements the RecordBuffer class for parsing CSV records.

#include "RecordBuffer.h"
#include <sstream>

RecordBuffer::RecordBuffer(const std::string& record) {
    std::stringstream ss(record);
    std::string field;
    while (getline(ss, field, ',')) {
        fields.push_back(field);
    }
}

std::string RecordBuffer::getKey() const {
    return fields.empty() ? "" : fields[0];
}

std::vector<std::string> RecordBuffer::getFields() const {
    return fields;
}
