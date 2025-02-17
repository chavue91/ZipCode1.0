/// @file CSVBuffer.cpp
/// @brief Implementation of CSVBuffer class.
/// @details This class reads a CSV file, processes Zip Code data, and generates a state-wise summary.

#include "CSVBuffer.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
#include <sstream>
#include <stdexcept>
#include <limits>

/// @brief Constructor that opens and reads a CSV file.
/// @param filename Name of the CSV file.
CSVBuffer::CSVBuffer(const string& filename) {
    file.open(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file");
    }
    loadRecords();
}

/// @brief Loads records from the CSV file into memory.
/// @details Reads each row from the CSV file, extracts zip code data,
///          and stores it in a vector of ZipRecord structures.
void CSVBuffer::loadRecords() {
    string line;
    getline(file, line); // Skip header row

    while (getline(file, line)) {
        stringstream ss(line);
        ZipRecord record;
        string temp;

        getline(ss, temp, ',');
        cout << "Reading ZIP Code: [" << temp << "]" << endl;  // Debugging output

        if (temp.empty()) {
            cerr << "Error: Empty ZIP code field found!" << endl;
            continue;  // Skip this line
        }

        try {
            record.zipCode = stoi(temp);  // Convert to integer
        } catch (const invalid_argument& e) {
            cerr << "Error: stoi failed due to invalid argument: " << temp << endl;
            continue;
        } catch (const out_of_range& e) {
            cerr << "Error: stoi failed due to out-of-range value: " << temp << endl;
            continue;
        }

        getline(ss, record.placeName, ',');
        getline(ss, record.state, ',');
        getline(ss, record.county, ',');
        getline(ss, temp, ',');
        
        try {
            record.latitude = stod(temp);
        } catch (...) {
            cerr << "Error: stod failed on latitude value: " << temp << endl;
            continue;
        }

        getline(ss, temp, ',');
        try {
            record.longitude = stod(temp);
        } catch (...) {
            cerr << "Error: stod failed on longitude value: " << temp << endl;
            continue;
        }

        records.push_back(record);
    }
}


/// @brief Retrieves all records.
/// @return A constant reference to a vector of ZipRecord structures.
const vector<ZipRecord>& CSVBuffer::getRecords() const {
    return records;
}

/// @brief Generates and prints a table of extreme Zip Codes for each state.
/// @details The user chooses a sorting field before processing data. The results
///          are stored in a user-specified output file.
/// @param sortField The column name by which the records will be sorted.
///        Acceptable values: "zipCode", "placeName", "state", "latitude", "longitude".
/// @param outputFilename The name of the file where the processed data will be saved.
/// @return Outputs the results to a file but does not return a value.
void CSVBuffer::generateStateTable() const {
    vector<ZipRecord> sortedRecords = records;  // Create a copy to sort
    string sortField;
    cout << "Choose sorting field (zipCode, placeName, state, latitude, longitude): ";
    cin >> sortField;
    
    if (sortField == "zipCode") {
        sort(sortedRecords.begin(), sortedRecords.end(), [](const ZipRecord& a, const ZipRecord& b) {
            return a.zipCode < b.zipCode;
        });
    } else if (sortField == "placeName") {
        sort(sortedRecords.begin(), sortedRecords.end(), [](const ZipRecord& a, const ZipRecord& b) {
            return a.placeName < b.placeName;
        });
    } else if (sortField == "state") {
        sort(sortedRecords.begin(), sortedRecords.end(), [](const ZipRecord& a, const ZipRecord& b) {
            return a.state < b.state;
        });
    } else if (sortField == "latitude") {
        sort(sortedRecords.begin(), sortedRecords.end(), [](const ZipRecord& a, const ZipRecord& b) {
            return a.latitude < b.latitude;
        });
    } else if (sortField == "longitude") {
        sort(sortedRecords.begin(), sortedRecords.end(), [](const ZipRecord& a, const ZipRecord& b) {
            return a.longitude < b.longitude;
        });
    } else {
        cerr << "Invalid sorting field. Defaulting to state." << endl;
    }

    map<string, tuple<ZipRecord, ZipRecord, ZipRecord, ZipRecord>> stateData;
    for (const auto& record : sortedRecords) {
        auto& [east, west, north, south] = stateData[record.state];
        if (east.zipCode == 0 || record.longitude < east.longitude) east = record;
        if (west.zipCode == 0 || record.longitude > west.longitude) west = record;
        if (north.zipCode == 0 || record.latitude > north.latitude) north = record;
        if (south.zipCode == 0 || record.latitude < south.latitude) south = record;
    }

    string outputFilename;
    cout << "Enter output filename: ";
    cin >> outputFilename;

    ofstream outFile(outputFilename);
    if (!outFile.is_open()) {
        cerr << "Error: Could not open output file." << endl;
        return;
    }

    outFile << "State, Easternmost, Westernmost, Northernmost, Southernmost" << endl;
    for (const auto& [state, extremes] : stateData) {
        const auto& [east, west, north, south] = extremes;
        outFile << state << ", " << east.zipCode << ", " << west.zipCode << ", "
                << north.zipCode << ", " << south.zipCode << endl;
    }

    outFile.close();
    cout << "Summary written to " << outputFilename << endl;
}
