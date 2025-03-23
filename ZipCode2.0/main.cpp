/// @file main.cpp
/// @brief Main application to process Zip Code data from CSV or length-indicated file.
/// @details Uses CSVBuffer to generate a state-wise table of extreme Zip Codes or convert CSV to length-indicated format.

#include "CSVBuffer.h"
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

/**
 * @brief Main function of the program.
 * @details Handles CSV conversion, summary generation, and zip code lookups via -z##### flags.
 * @return int Returns 0 on success, 1 on failure.
 */
int main(int argc, char* argv[]) {
    // Check for zip lookup flags
    vector<int> zipCodes;
    string inputFile;
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-z", 2) == 0 || strncmp(argv[i], "-Z", 2) == 0) {
            try {
                zipCodes.push_back(stoi(argv[i] + 2));
            } catch (...) {
                cerr << "Invalid zip code argument: " << argv[i] << endl;
                return 1;
            }
        } else {
            inputFile = argv[i];
        }
    }

    if (!zipCodes.empty()) {
        if (inputFile.empty()) {
            cerr << "Error: Please specify the filename before zip code flags." << endl;
            return 1;
        }
        try {
            CSVBuffer buffer(inputFile);
            buffer.searchByZipCodes(zipCodes);
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
        return 0;
    }

    // No zip code flags: fallback to interactive menu
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

    return 0;
}