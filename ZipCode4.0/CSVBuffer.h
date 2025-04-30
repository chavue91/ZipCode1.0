/// @file main.cpp
/// @brief Main application to process Zip Code data from CSV or length-indicated file.
/// @details Uses CSVBuffer to generate a state-wise table of extreme Zip Codes or convert CSV to length-indicated format.

#ifndef CSVBUFFER_H
#define CSVBUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <unordered_map>

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
    void loadRecords();

    /// @brief Loads length-indicated records from a file.
    void loadLengthIndicatedRecords();

public:
    /// @brief Constructor that opens a CSV or length-indicated file.
    /// @param filename Name of the input file.
    CSVBuffer(const string& filename);

    /// @brief Retrieves all zip code records.
    /// @return A constant reference to a vector containing all ZipRecord structures.
    const vector<ZipRecord>& getRecords() const;

    /// @brief Generates a state-wise summary of extreme zip codes.
    /// @details Finds and outputs the easternmost, westernmost, northernmost, and southernmost zip codes per state.
    void generateStateTable() const;

    /// @brief Converts a standard CSV file to a length-indicated format.
    /// @param inputFile The input CSV filename.
    /// @param outputFile The output filename in length-indicated format.
    static void convertCSVToLengthIndicated(const string& inputFile, const string& outputFile);

    /// @brief Builds an in-memory primary key index of zip codes.
    /// @return An unordered map from zip code to ZipRecord.
    unordered_map<int, ZipRecord> buildPrimaryKeyIndex() const;

    /// @brief Searches and prints zip code records from a list.
    /// @param zipCodes Vector of zip codes to look up.
    void searchByZipCodes(const vector<int>& zipCodes) const;
};

#endif // CSVBUFFER_H
