/// @file CSVBuffer.h
/// @brief Defines a buffer class for handling CSV data.
/// @details This class reads a CSV file and allows extraction of structured data.

#ifndef CSVBUFFER_H
#define CSVBUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>

using namespace std;

struct ZipRecord {
    int zipCode;
    string placeName;
    string state;
    string county;
    double latitude;
    double longitude;
};

class CSVBuffer {
private:
    ifstream file;
    vector<ZipRecord> records;
    void loadRecords();
public:
    /// @brief Constructor that opens a CSV file.
    /// @param filename Name of the CSV file.
    CSVBuffer(const string& filename);
    
    /// @brief Retrieves all records.
    /// @return Vector of ZipRecord structures.
    const vector<ZipRecord>& getRecords() const;
    
    /// @brief Generates and prints a table of extreme Zip Codes for each state.
    void generateStateTable() const;
};

#endif // CSVBUFFER_H
