/// @file main.cpp
/// @brief Main application to process Zip Code data from CSV or length-indicated file.
/// @details Uses CSVBuffer to generate a state-wise table of extreme Zip Codes or convert CSV to length-indicated format.

#include "CSVBuffer.h"
#include <iostream>

using namespace std;

/**
 * @brief Main function of the program.
 * @details Prompts the user to choose between converting a CSV file to length-indicated format or processing a file.
 * @return int Returns 0 on success, 1 on failure.
 */
int main() {
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
