/// @file main.cpp
/// @brief Main application to process Zip Code data from CSV file.
/// @details Uses CSVBuffer to generate a state-wise table of extreme Zip Codes.

#include "CSVBuffer.h"
#include <iostream>

using namespace std;

/**
 * @brief Main function of the program.
 * @details Prompts the user for a CSV file containing zip code data, processes the data,
 *          and generates a state-wise summary of extreme zip codes.
 * @return int Returns 0 on successful execution, 1 on failure.
 */
int main() {
    string zipcode; ///< Variable to store the name of the user-provided CSV file.
    
    // Prompt user for input CSV filename
    cout << "Enter a zipcode csv file: ";
    cin >> zipcode;
    
    try {
        string filename = zipcode; ///< Stores the filename provided by the user.
        CSVBuffer buffer(filename); ///< Creates a CSVBuffer object to process the file.
        buffer.generateStateTable(); ///< Calls function to generate and save the state-wise zip code summary.
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl; ///< Prints error message if an exception is caught.
        return 1; ///< Returns 1 to indicate failure.
    }
    
    return 0; ///< Returns 0 to indicate successful execution.
}
