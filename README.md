# ZipCode1.0
# Zip Code Data Processor

## Description

This program reads a CSV file containing ZIP code information and processes it to generate a state-wise summary of extreme ZIP codes (Easternmost, Westernmost, Northernmost, and Southernmost). The results are stored in an output file named `state_zip_summary.csv`.

## Features

- Reads ZIP code data from a CSV file.
- Extracts relevant information such as state, latitude, and longitude.
- Determines the Easternmost, Westernmost, Northernmost, and Southernmost ZIP codes for each state.
- Saves the processed results to a CSV file (`state_zip_summary.csv`).
- Includes error handling for missing or malformed data.

## Files in the Project

- **`main.cpp`** - The main program that initializes and runs the data processing.
- **`CSVBuffer.h`** - The header file defining the `CSVBuffer` class.
- **`CSVBuffer.cpp`** - Implementation of the `CSVBuffer` class.
- **`zip_codes.csv`** - Sample input CSV file containing ZIP code data.
- **`state_zip_summary.csv`** - Output file storing the processed data.
- **`README.md`** - This documentation file.

## Compilation Instructions

### **Using g++ (Linux/macOS/Windows with MinGW)**

To compile the program, run:

```sh
 g++ -o myProgram main.cpp CSVBuffer.cpp
```

This command generates an executable file named `myProgram`.

## Running the Program

Once compiled, you can run the program as follows:

```sh
 ./myProgram
```

On Windows (if using MinGW):

```sh
 myProgram.exe
```

The program will prompt for a CSV filename. Enter the correct path to `zip_codes.csv`.

## Expected Output

The program generates a CSV file named `state_zip_summary.csv`, which contains:

```
State, Easternmost, Westernmost, Northernmost, Southernmost
NY, 10001, 14905, 10598, 10002
MA, 01001, 02703, 01350, 02535
...
```

## Error Handling

- If the CSV file is missing or unreadable, an error message will be displayed.
- If a row contains invalid data, it will be skipped, and a warning will be logged.

## Author

Cha Vue, Sofia Hoffman, Alexander Miller, Zoljargal Enkhbayar, Yohannes Niguesse, Fatha Abdi

