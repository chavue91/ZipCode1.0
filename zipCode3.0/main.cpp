/// @file main.cpp
/// @brief Main application to process Zip Code data using blocked sequence set structure.
/// @details Supports CSV conversion, summary generation, zip code search, record insertion,
/// deletion, and dump features for blocked sequence set files.

#include "CSVBuffer.h"
#include "DumpUtility.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
    vector<int> zipCodes;
    string inputFile;
    string indexFile = "zipcode.idx";
    string insertFile;
    string deleteFile;
    bool dumpPhysical = false;
    bool dumpLogical = false;
    bool runInsert = false;
    bool runDelete = false;
    bool runSearch = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg.rfind("-z", 0) == 0 || arg.rfind("-Z", 0) == 0) {
            try {
                zipCodes.push_back(stoi(arg.substr(2)));
                runSearch = true;
            } catch (...) {
                cerr << "Invalid zip code argument: " << arg << endl;
                return 1;
            }
        } else if (arg == "--dump-physical") {
            dumpPhysical = true;
        } else if (arg == "--dump-logical") {
            dumpLogical = true;
        } else if (arg == "--insert" && i + 1 < argc) {
            insertFile = argv[++i];
            runInsert = true;
        } else if (arg == "--delete" && i + 1 < argc) {
            deleteFile = argv[++i];
            runDelete = true;
        } else {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        cerr << "Error: Must specify input file." << endl;
        return 1;
    }

    if (runSearch && !zipCodes.empty()) {
        try {
            CSVBuffer buffer(inputFile);
            buffer.searchByZipCodes(zipCodes);
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    }

    if (dumpPhysical) {
        dumpByPhysicalOrder(inputFile);
    }

    if (dumpLogical) {
        dumpByLogicalOrder(inputFile);
    }

    if (runInsert && !insertFile.empty()) {
        string cmd = "./InsertRecord " + inputFile + " " + indexFile + " " + insertFile;
        system(cmd.c_str());
    }

    if (runDelete && !deleteFile.empty()) {
        string cmd = "./DeleteRecord " + inputFile + " " + indexFile + " " + deleteFile;
        system(cmd.c_str());
    }

    if (!runSearch && !runInsert && !runDelete && !dumpLogical && !dumpPhysical) {
        cout << "Choose operation:\n1. Convert CSV to length-indicated file\n2. Process file and generate summary\nEnter choice (1 or 2): ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            string inputFile, outputFile;
            cout << "Enter input CSV filename: ";
            cin >> inputFile;
            cout << "Enter output filename for length-indicated file: ";
            cin >> outputFile;
            CSVBuffer::convertCSVToLengthIndicated(inputFile, outputFile);
            cout << "Conversion complete.\n";
        } else if (choice == 2) {
            string inputFile;
            cout << "Enter filename to process: ";
            cin >> inputFile;
            try {
                CSVBuffer buffer(inputFile);
                buffer.generateStateTable();
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
                return 1;
            }
        } else {
            cout << "Invalid choice. Exiting.\n";
            return 1;
        }
    }

    return 0;
}