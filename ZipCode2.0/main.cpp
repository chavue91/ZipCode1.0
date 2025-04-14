/// @file main.cpp
/// @brief Main application to process Zip Code data from CSV or length-indicated file.
/// @details Uses CSVBuffer to generate a state-wise table of extreme Zip Codes,
/// convert CSV files to length-indicated format, or search records by zip code
/// using in-memory primary key indexing via command-line flags.

#include "CSVBuffer.h"
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

/**
 * @brief Main function of the program.
 * @details Handles CSV conversion, summary generation, and zip code lookups via -z##### flags.
 *          If zip code flags are provided, the program loads the file, builds an index,
 *          and prints matching records. If no flags are given, it enters interactive mode.
 * @param argc Argument count from command line.
 * @param argv Array of command line arguments.
 * @return int Returns 0 on success, 1 on failure.
 */
int main(int argc, char* argv[]) {
    vector<int> zipCodes; ///< Stores zip codes provided via -z##### command-line flags.
    string inputFile;     ///< Filename provided to load zip code data from.

    /**
     * @brief Parse command-line arguments.
     * @details Extract zip code flags and the data file to be processed.
     */
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-z", 2) == 0 || strncmp(argv[i], "-Z", 2) == 0) {
            try {
                zipCodes.push_back(stoi(argv[i] + 2)); ///< Extracts and stores zip code number.
            } catch (...) {
                cerr << "Invalid zip code argument: " << argv[i] << endl;
                return 1;
            }
        } else {
            inputFile = argv[i]; ///< Assumes first non-flag argument is the filename.
        }
    }

    /**
     * @brief If zip lookup flags were provided, perform index search.
     */
    if (!zipCodes.empty()) {
        if (inputFile.empty()) {
            cerr << "Error: Please specify the filename before zip code flags." << endl;
            return 1;
        }
        try {
            CSVBuffer buffer(inputFile); ///< Initializes buffer and loads records.
            buffer.searchByZipCodes(zipCodes); ///< Searches for each zip code.
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
        return 0;
    }

    /**
     * @brief If no flags, show interactive options for CSV conversion or summary.
     */
    cout << "Choose operation:\n1. Convert CSV to length-indicated file\n2. Process file and generate summary\nEnter choice (1 or 2): ";
    int choice; ///< User-selected operation.
    cin >> choice;

    /**
     * @brief Option 1 - Convert CSV to length-indicated format.
     */
    if (choice == 1) {
        string inputFile, outputFile; ///< Filenames for conversion.
        cout << "Enter input CSV filename: ";
        cin >> inputFile;
        cout << "Enter output filename for length-indicated file: ";
        cin >> outputFile;
        CSVBuffer::convertCSVToLengthIndicated(inputFile, outputFile); ///< Perform conversion.
        cout << "Conversion complete.\n";
    }
    /**
     * @brief Option 2 - Generate summary of extremes per state.
     */
    else if (choice == 2) {
        string inputFile; ///< Filename for summary generation.
        cout << "Enter filename to process: ";
        cin >> inputFile;
        try {
            CSVBuffer buffer(inputFile); ///< Loads zip code records.
            buffer.generateStateTable(); ///< Generates summary report.
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    } else {
        cout << "Invalid choice. Exiting.\n";
        return 1;
    }

    return 0; ///< Return 0 on success.
}

