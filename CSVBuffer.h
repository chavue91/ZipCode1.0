/// @file CSVBuffer.h
/// @brief Defines a buffer class for handling CSV data.
/// @details This class reads a CSV file, processes zip code data,
///          and generates a state-wise summary of extreme zip codes.

#ifndef CSVBUFFER_H
#define CSVBUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>

using namespace std;

/// @brief Structure to store zip code information.
struct ZipRecord {
    int zipCode;       ///< Zip code number.
    string placeName;  ///< Name of the place.
    string state;      ///< Two-letter state abbreviation.
    string county;     ///< County name.
    double latitude;   ///< Latitude coordinate.
    double longitude;  ///< Longitude coordinate.
};

/// @brief Class to handle CSV data and extract structured zip code records.
class CSVBuffer {
private:
    ifstream file;           ///< File stream to read CSV data.
    vector<ZipRecord> records; ///< Vector to store all zip code records.

    /// @brief Loads records from the CSV file into memory.
    /// @details Reads each row from the CSV file and populates the vector of ZipRecord structures.
    void loadRecords();

public:
    /// @brief Constructor that opens a CSV file.
    /// @param filename Name of the CSV file.
    CSVBuffer(const string& filename);
    
    /// @brief Retrieves all zip code records.
    /// @return A constant reference to a vector containing all ZipRecord structures.
    const vector<ZipRecord>& getRecords() const;
    
    /// @brief Generates a state-wise summary of extreme zip codes.
    /// @details Finds and outputs the easternmost, westernmost, northernmost, and southernmost zip codes per state.
    /// @return Outputs the summary to a file but does not return a value.
    void generateStateTable() const;
};

#endif // CSVBUFFER_H
