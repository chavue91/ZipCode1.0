#include "CSVBuffer.h"
#include "HeaderBuffer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

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
    ofstream out(outputFile);

    if (!in.is_open() || !out.is_open()) {
        cerr << "Error opening files." << endl;
        return;
    }

    string headerLine;
    getline(in, headerLine);

    vector<Field> fields = {
        {"zipCode", "int", "fixed", true},
        {"placeName", "string", "variable"},
        {"state", "string", "fixed"},
        {"county", "string", "variable"},
        {"latitude", "double", "fixed"},
        {"longitude", "double", "fixed"}
    };

    Header header("LengthIndicated", "1.0", 0, "ASCII", "zipcode.idx", 0, fields, "zipCode");
    header.writeToFile(out);

    string line;
    while (getline(in, line)) {
        int length = line.size();
        out << setw(4) << setfill('0') << length << "," << line << endl;
    }

    in.close();
    out.close();
}

const vector<ZipRecord>& CSVBuffer::getRecords() const {
    return records;
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
