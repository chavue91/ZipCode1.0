/// @file main.cpp
/// @brief Main application to process Zip Code data from CSV file.
/// @details Uses CSVBuffer to generate a state-wise table of extreme Zip Codes.

#include "CSVBuffer.h"
#include <iostream>

using namespace std;

int main() {
    string zipcode;
    cout << "Enter a zipcode csv file: ";
    cin >> zipcode;
    try {
        string filename = zipcode;
        CSVBuffer buffer(filename);
        buffer.generateStateTable();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
