/// @file main.cpp
/// @brief Main application to process Zip Code data from CSV or length-indicated file.
/// @details Uses CSVBuffer to generate a state-wise table of extreme Zip Codes,
/// convert CSV files to length-indicated format, or search records by zip code
/// using in-memory primary key indexing via command-line flags.

#include "CSVBuffer.h"
#include "BPlusTree.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <limits>
#include <iomanip>

using namespace std;

/**
 * @brief Prints a ZIP code record with formatted output.
 * @param record The ZipRecord to print.
 */
void printZipRecord(const ZipRecord& record) {
    cout << "ZIP Code: " << record.zipCode << endl;
    cout << "Place Name: " << record.placeName << endl;
    cout << "State: " << record.state << endl;
    cout << "County: " << record.county << endl;
    cout << "Latitude: " << fixed << setprecision(6) << record.latitude << endl;
    cout << "Longitude: " << fixed << setprecision(6) << record.longitude << endl;
}

// Global variables for record traversal
int g_recordCount = 0;
int g_recordLimit = 0;

/**
 * @brief Callback function for traversing records in a B+Tree.
 * @param record The ZipRecord being traversed.
 */
void traversalCallback(const ZipRecord& record) {
    if (g_recordCount < g_recordLimit) {
        cout << endl << "Record " << (g_recordCount + 1) << ":" << endl;
        printZipRecord(record);
        cout << string(50, '-') << endl;
        g_recordCount++;
    }
}

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
    cout << "Choose operation:\n"
         << "1. Convert CSV to length-indicated file\n"
         << "2. Process file and generate summary\n"
         << "3. Print B+Tree structure\n"
         << "4. Insert a record into B+Tree\n"
         << "5. Delete a record from B+Tree\n"
         << "6. Create new B+Tree from CSV\n"
         << "7. Search record in B+Tree\n"
         << "Enter choice (1-7): ";
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
    }
    /**
     * @brief Option 3 - Print B+Tree structure.
     */
    else if (choice == 3) {
        string inputFile; ///< Filename for tree visualization.
        cout << "Enter filename to process: ";
        cin >> inputFile;
        try {
            // Open the B+Tree file
            BPlusTree tree(inputFile);
            
            // Print the tree structure
            tree.printTree();
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    }
    /**
     * @brief Option 4 - Insert a record into B+Tree.
     */
    else if (choice == 4) {
        string inputFile;
        cout << "Enter B+Tree filename: ";
        cin >> inputFile;
        
        try {
            // Open the B+Tree file
            BPlusTree tree(inputFile);
            
            // Get record details from user
            ZipRecord record;
            cout << "Enter ZIP code: ";
            cin >> record.zipCode;
            
            cout << "Enter place name: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear newline
            getline(cin, record.placeName);
            
            cout << "Enter state (2-letter code): ";
            cin >> record.state;
            
            cout << "Enter county: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear newline
            getline(cin, record.county);
            
            cout << "Enter latitude: ";
            cin >> record.latitude;
            
            cout << "Enter longitude: ";
            cin >> record.longitude;
            
            // Insert the record
            bool success = tree.insert(record);
            if (success) {
                cout << "Record inserted successfully!" << endl;
                
                // Optionally print the tree to verify the insertion
                char printOption;
                cout << "Print the updated tree structure? (y/n): ";
                cin >> printOption;
                if (printOption == 'y' || printOption == 'Y') {
                    tree.printTree();
                }
            } else {
                cout << "Failed to insert record." << endl;
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    }
    /**
     * @brief Option 5 - Delete a record from B+Tree.
     */
    else if (choice == 5) {
        string inputFile;
        cout << "Enter B+Tree filename: ";
        cin >> inputFile;
        
        try {
            // Open the B+Tree file
            BPlusTree tree(inputFile);
            
            // Get ZIP code to delete
            int zipCode;
            cout << "Enter ZIP code to delete: ";
            cin >> zipCode;
            
            // First verify the record exists
            ZipRecord record = tree.search(zipCode);
            if (record.zipCode != 0) {
                cout << "Found record to delete:" << endl;
                cout << "ZIP Code: " << record.zipCode 
                     << ", Place: " << record.placeName 
                     << ", State: " << record.state << endl;
                
                // Confirm deletion
                char confirm;
                cout << "Confirm deletion? (y/n): ";
                cin >> confirm;
                
                if (confirm == 'y' || confirm == 'Y') {
                    // Delete the record
                    bool success = tree.remove(zipCode);
                    if (success) {
                        cout << "Record deleted successfully!" << endl;
                        
                        // Optionally print the tree to verify the deletion
                        char printOption;
                        cout << "Print the updated tree structure? (y/n): ";
                        cin >> printOption;
                        if (printOption == 'y' || printOption == 'Y') {
                            tree.printTree();
                        }
                    } else {
                        cout << "Failed to delete record." << endl;
                    }
                } else {
                    cout << "Deletion cancelled." << endl;
                }
            } else {
                cout << "ZIP code " << zipCode << " not found in the B+Tree." << endl;
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    }
    /**
     * @brief Option 6 - Create a new B+Tree from CSV.
     */
    else if (choice == 6) {
        string csvFile, btreeFile;
        cout << "Enter input CSV filename: ";
        cin >> csvFile;
        cout << "Enter output B+Tree filename: ";
        cin >> btreeFile;
        
        try {
            // Create a new B+Tree file
            cout << "Creating new B+Tree file..." << endl;
            BPlusTree tree(btreeFile, 4096, true);
            
            // Get tree parameters
            int order, recordsPerBlock;
            cout << "Enter order of the B+Tree (recommended: 10): ";
            cin >> order;
            cout << "Enter records per sequence block (recommended: 20): ";
            cin >> recordsPerBlock;
            
            // Initialize with specified parameters
            tree.initialize(order, recordsPerBlock);
            
            // Bulk load from CSV
            cout << "Loading data from " << csvFile << "..." << endl;
            tree.bulkLoad(csvFile);
            
            cout << "B+Tree created successfully!" << endl;
            cout << "Tree height: " << tree.getTreeHeight() << endl;
            cout << "Total blocks: " << tree.getBlockCount() << endl;
            
            // Optionally print the tree structure
            char printOption;
            cout << "Print the tree structure? (y/n): ";
            cin >> printOption;
            if (printOption == 'y' || printOption == 'Y') {
                tree.printTree();
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    }
    /**
     * @brief Option 7 - Search for a record in B+Tree.
     */
    else if (choice == 7) {
        string inputFile;
        cout << "Enter B+Tree filename: ";
        cin >> inputFile;
        
        try {
            // Open the B+Tree file
            BPlusTree tree(inputFile);
            
            // Choose search method
            cout << "Search methods:\n"
                 << "1. Search by ZIP code\n"
                 << "2. Display first N records\n"
                 << "Enter choice (1 or 2): ";
            int searchMethod;
            cin >> searchMethod;
            
            if (searchMethod == 1) {
                // Search by ZIP code
                int zipCode;
                cout << "Enter ZIP code to search: ";
                cin >> zipCode;
                
                ZipRecord record = tree.search(zipCode);
                
                if (record.zipCode != 0) {
                    cout << "\nRecord found:\n" << string(50, '-') << endl;
                    printZipRecord(record);
                } else {
                    cout << "ZIP code " << zipCode << " not found in the B+Tree." << endl;
                }
            } 
            else if (searchMethod == 2) {
                // Display first N records
                cout << "Enter number of records to display: ";
                cin >> g_recordLimit;
                
                cout << "\nFirst " << g_recordLimit << " records in the B+Tree:\n" << string(50, '-') << endl;
                
                // Reset the counter
                g_recordCount = 0;
                
                // Traverse the records
                tree.traverseInOrder(traversalCallback);
                
                cout << "\nDisplayed " << g_recordCount << " records." << endl;
            }
            else {
                cout << "Invalid search method. Exiting." << endl;
                return 1;
            }
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