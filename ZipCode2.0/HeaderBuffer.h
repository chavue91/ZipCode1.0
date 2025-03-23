/// @file HeaderBuffer.h
/// @brief Defines a class for handling header records in data files.
/// @details This class provides functionality to read and write the header record 
///          of a length-indicated CSV file, supporting structured metadata.

#ifndef HEADERBUFFER_H
#define HEADERBUFFER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

/// @brief Structure to represent a field in the header.
struct Field {
    string name;               ///< Field name
    string type;               ///< Data type of the field
    string format;             ///< Format of the field data
    bool isPrimaryKey = false; ///< Indicates if this field is the primary key
};

/// @brief Class to manage header metadata for record-based file storage.
class Header {
public:
    string fileStructureType;
    string version;
    int sizeInBytes;
    string sizeFormatType;
    string primaryKeyFileName;
    int recordCount;
    int fieldsPerRecord;
    vector<Field> fields;
    string primaryKey;

    /// @brief Default constructor initializing fields to default values.
    Header()
        : fileStructureType(""), version(""), sizeInBytes(0), sizeFormatType("ASCII"),
          primaryKeyFileName(""), recordCount(0), fieldsPerRecord(0), primaryKey("") {}

    /// @brief Parameterized constructor to initialize with custom values.
    Header(const string& fileStructureType, const string& version, int sizeInBytes,
           const string& sizeFormatType, const string& primaryKeyFileName, 
           int recordCount, const vector<Field>& fields, const string& primaryKey)
        : fileStructureType(fileStructureType), version(version), sizeInBytes(sizeInBytes),
          sizeFormatType(sizeFormatType), primaryKeyFileName(primaryKeyFileName),
          recordCount(recordCount), fieldsPerRecord(fields.size()),
          fields(fields), primaryKey(primaryKey) {}

    /// @brief Writes the header to a file.
    /// @param out Output file stream to write the structured header.
    void writeToFile(ofstream& out) const {
        if (!out.is_open()) {
            cerr << "Error: Output file is not open." << endl;
            return;
        }

        out << "HEADER,";
        out << fileStructureType << "," << version << "," << sizeInBytes << "," << sizeFormatType << ","
            << primaryKeyFileName << "," << recordCount << "," << fieldsPerRecord << "," << primaryKey << "\n";

        for (const auto& field : fields) {
            out << field.name << "," << field.type << "," << field.format << ","
                << (field.isPrimaryKey ? "true" : "false") << "\n";
        }
    }

    /// @brief Reads the header from a file.
    /// @param in Input file stream to read the structured header.
    /// @return True if successfully read, false otherwise.
    bool readFromFile(ifstream& in) {
        if (!in.is_open()) {
            cerr << "Error: Input file is not open." << endl;
            return false;
        }

        string line;
        if (!getline(in, line)) return false;

        if (line.rfind("HEADER,", 0) != 0) {
            cerr << "Error: Missing HEADER line." << endl;
            return false;
        }

        stringstream ss(line.substr(7));  // skip "HEADER,"
        string temp;

        getline(ss, fileStructureType, ',');
        getline(ss, version, ',');
        getline(ss, temp, ','); sizeInBytes = stoi(temp);
        getline(ss, sizeFormatType, ',');
        getline(ss, primaryKeyFileName, ',');
        getline(ss, temp, ','); recordCount = stoi(temp);
        getline(ss, temp, ','); fieldsPerRecord = stoi(temp);
        getline(ss, primaryKey, ',');

        fields.clear();
        for (int i = 0; i < fieldsPerRecord; ++i) {
            if (!getline(in, line)) break;
            stringstream fss(line);
            Field f;
            string isPK;
            getline(fss, f.name, ',');
            getline(fss, f.type, ',');
            getline(fss, f.format, ',');
            getline(fss, isPK, ',');
            f.isPrimaryKey = (isPK == "true");
            fields.push_back(f);
        }

        cout << "Header loaded:\n"
             << "Structure: " << fileStructureType << "\n"
             << "Version: " << version << "\n"
             << "Fields per Record: " << fieldsPerRecord << "\n"
             << "Primary Key: " << primaryKey << endl;

        return true;
    }
};

#endif // HEADERBUFFER_H
