/// @file RecordBuffer.h
/// @brief Provides a utility class to unpack a record string into fields.

#ifndef RECORDBUFFER_H
#define RECORDBUFFER_H

#include <string>
#include <vector>

/// @brief RecordBuffer class extracts fields from a comma-separated record.
class RecordBuffer {
public:
    /// @brief Constructs a RecordBuffer with a record string.
    /// @param record Comma-separated field values.
    RecordBuffer(const std::string& record);

    /// @brief Returns the value of the key field (assumed to be first).
    /// @return Key field as a string.
    std::string getKey() const;

    /// @brief Returns all fields unpacked from the record.
    /// @return Vector of string fields.
    std::vector<std::string> getFields() const;

private:
    std::vector<std::string> fields;
};

#endif // RECORDBUFFER_H
