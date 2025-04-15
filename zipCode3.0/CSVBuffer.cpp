/// @file CSVBuffer.cpp
/// @brief Implementation of CSVBuffer class functions.
/// @details This file provides functionality for reading CSV or length-indicated ZIP code data,
/// building a primary key index, converting CSV to a structured format, and generating summaries.

#include "CSVBuffer.h"
#include "HeaderBuffer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <cstdio>

using namespace std;

CSVBuffer::CSVBuffer(const string& filename) {
    file.open(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file");
    }

    Header header;
    if (header.readFromFile(file)) {
        cout << "Header successfully loaded from file." << endl;
        loadLengthIndicatedRecords();
    } else {
        loadRecords();
    }
}

unordered_map<int, ZipRecord> CSVBuffer::buildPrimaryKeyIndex() const {
    unordered_map<int, ZipRecord> index;
    for (const auto& record : records) {
        index[record.zipCode] = record;
    }
    return index;
}

void CSVBuffer::searchByZipCodes(const vector<int>& zipCodes) const {
    auto index = buildPrimaryKeyIndex();
    for (int zip : zipCodes) {
        auto it = index.find(zip);
        if (it != index.end()) {
            const auto& r = it->second;
            cout << "Zip Code: " << r.zipCode << ", Place Name: " << r.placeName
                 << ", State: " << r.state << ", County: " << r.county
                 << ", Latitude: " << r.latitude << ", Longitude: " << r.longitude << endl;
        } else {
            cout << "Zip Code " << zip << " not found in the file." << endl;
        }
    }
}

void CSVBuffer::loadRecords() {
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        ZipRecord record;
        string temp;

        getline(ss, temp, ',');
        if (temp.empty()) continue;
        try { record.zipCode = stoi(temp); } catch (...) { continue; }
        getline(ss, record.placeName, ',');
        getline(ss, record.state, ',');
        getline(ss, record.county, ',');

        getline(ss, temp, ',');
        try { record.latitude = stod(temp); } catch (...) { continue; }
        getline(ss, temp, ',');
        try { record.longitude = stod(temp); } catch (...) { continue; }

        records.push_back(record);
    }
}

void CSVBuffer::loadLengthIndicatedRecords() {
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string lenStr, data;
        getline(ss, lenStr, ',');
        getline(ss, data);

        stringstream dataStream(data);
        ZipRecord record;
        string temp;

        getline(dataStream, temp, ',');
        try { record.zipCode = stoi(temp); } catch (...) { continue; }
        getline(dataStream, record.placeName, ',');
        getline(dataStream, record.state, ',');
        getline(dataStream, record.county, ',');

        getline(dataStream, temp, ',');
        try { record.latitude = stod(temp); } catch (...) { continue; }
        getline(dataStream, temp, ',');
        try { record.longitude = stod(temp); } catch (...) { continue; }

        records.push_back(record);
    }
}

void CSVBuffer::convertCSVToLengthIndicated(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile);
    if (!in.is_open()) {
        cerr << "Error opening input file." << endl;
        return;
    }

    ofstream tempOut("temp_data.tmp");
    if (!tempOut.is_open()) {
        cerr << "Error creating temporary file." << endl;
        return;
    }

    string headerLine;
    getline(in, headerLine);
    headerLine.erase(remove(headerLine.begin(), headerLine.end(), '"'), headerLine.end());
    headerLine.erase(remove(headerLine.begin(), headerLine.end(), '\n'), headerLine.end());

    stringstream ss(headerLine);
    string fieldName;
    vector<Field> fields;
    int index = 0;
    int primaryKeyIndex = 0;

    cout << "Parsed Fields:" << endl;
    while (getline(ss, fieldName, ',')) {
        string rawField = fieldName;
        fieldName.erase(remove_if(fieldName.begin(), fieldName.end(), ::isspace), fieldName.end());

        Field field;
        field.name = fieldName;
        field.type = "string";
        field.format = "text/csv";
        field.isPrimaryKey = false;
        fields.push_back(field);

        cout << "  Field " << index << ": '" << rawField << "' -> '" << fieldName << "'" << endl;
        ++index;
    }

    if (!fields.empty()) {
        fields[0].isPrimaryKey = true;
        primaryKeyIndex = 0;
    }

    int sizeInBytes = 50;
    for (const auto& field : fields) {
        sizeInBytes += field.name.length() + field.type.length() + field.format.length() + 10;
    }

    int recordCount = 0;
    string line;
    while (getline(in, line)) {
        line.erase(remove(line.begin(), line.end(), '"'), line.end());
        if (line.empty()) continue;

        int length = line.size();
        tempOut << setw(4) << setfill('0') << length << "," << line << endl;
        ++recordCount;
    }

    in.close();
    tempOut.close();

    ofstream out(outputFile);
    if (!out.is_open()) {
        cerr << "Error creating output file." << endl;
        return;
    }

    Header header("LengthIndicated", "1.0", sizeInBytes, "ASCII", "zipcode.idx",
                  recordCount, fields, fields[primaryKeyIndex].name);
    header.writeToFile(out);

    ifstream tempIn("temp_data.tmp");
    out << tempIn.rdbuf();
    tempIn.close();
    out.close();
    remove("temp_data.tmp");
}

void CSVBuffer::generateStateTable() const {
    vector<ZipRecord> sortedRecords = records;
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
