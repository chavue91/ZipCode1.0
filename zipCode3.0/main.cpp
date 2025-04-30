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

/**
 * @brief Main driver function.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code.
 */
int main(int argc, char* argv[]) {
    vector<int> zipCodes;              ///< List of zip codes for search.
    string inputFile;                  ///< Input data file name.
    string indexFile = "zipcode.idx"; ///< Default index file name.
    string insertFile;                 ///< File containing records to insert.
    string deleteFile;                 ///< File containing records to delete.
    bool dumpPhysical = false;         ///< Flag for physical dump.
    bool dumpLogical = false;          ///< Flag for logical dump.
    bool runInsert = false;            ///< Flag for insertion.
    bool runDelete = false;            ///< Flag for deletion.
    bool runSearch = false;            ///< Flag for zip code search.

    /**
     * @brief Parse command-line arguments.
     */
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg.rfind("-z", 0) == 0 || arg.rfind("-Z", 0) == 0) {
            try {
                zipCodes.push_back(stoi(arg.substr(2))); ///< Add zip code from argument.
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
            inputFile = arg; ///< Assume non-flag argument is input file.
        }
    }

    /// Validate that an input file was specified.
    if (inputFile.empty()) {
        cerr << "Error: Must specify input file." << endl;
        return 1;
    }

    /// Perform zip code search if applicable.
    if (runSearch && !zipCodes.empty()) {
        try {
            CSVBuffer buffer(inputFile); ///< Load file into CSVBuffer.
            buffer.searchByZipCodes(zipCodes); ///< Perform zip code search.
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    }

    /// Dump blocks in physical order if flag set.
    if (dumpPhysical) {
        dumpByPhysicalOrder(inputFile);
    }

    /// Dump blocks in logical order if flag set.
    if (dumpLogical) {
        dumpByLogicalOrder(inputFile);
    }

    /// Run InsertRecord program if insertion requested.
    if (runInsert && !insertFile.empty()) {
        string cmd = "./InsertRecord " + inputFile + " " + indexFile + " " + insertFile;
        system(cmd.c_str()); ///< Execute external insert program.
    }

    /// Run DeleteRecord program if deletion requested.
    if (runDelete && !deleteFile.empty()) {
        string cmd = "./DeleteRecord " + inputFile + " " + indexFile + " " + deleteFile;
        system(cmd.c_str()); ///< Execute external delete program.
    }

    /// If no arguments matched, enter interactive mode.
    if (!runSearch && !runInsert && !runDelete && !dumpLogical && !dumpPhysical) {
        cout << "Choose operation:\n1. Convert CSV to length-indicated file\n2. Process file and generate summary\nEnter choice (1 or 2): ";
        int choice;
        cin >> choice;

        /// Option 1: Convert CSV to length-indicated format.
        if (choice == 1) {
            string inputFile, outputFile;
            cout << "Enter input CSV filename: ";
            cin >> inputFile;
            cout << "Enter output filename for length-indicated file: ";
            cin >> outputFile;
            CSVBuffer::convertCSVToLengthIndicated(inputFile, outputFile);
            cout << "Conversion complete.\n";
        }
        /// Option 2: Generate extreme zip summary per state.
        else if (choice == 2) {
            string inputFile;
            cout << "Enter filename to process: ";
            cin >> inputFile;
            try {
                CSVBuffer buffer(inputFile);
                buffer.generateStateTable(); ///< Generate summary report.
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
                return 1;
            }
        } else {
            cout << "Invalid choice. Exiting.\n";
            return 1;
        }
    }

    return 0; ///< Success.
}
