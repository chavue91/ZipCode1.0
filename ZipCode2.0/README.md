# ZipCode2.0  
# Zip Code Data Processor with Index Support

## Description

This program processes U.S. ZIP code data from a CSV or length-indicated file and supports both:
- Generating a state-wise summary of extreme ZIP codes.
- Searching ZIP codes using a fast in-memory primary key index via command-line flags.

## Features

- Reads ZIP code data from a standard CSV or length-indicated format.
- Converts CSV to a length-indicated file with a structured header.
- Extracts fields such as zip code, city, state, latitude, and longitude.
- Generates a summary of easternmost, westernmost, northernmost, and southernmost ZIP codes per state.
- Creates and uses a **primary key index** in RAM for fast ZIP code lookups.
- Accepts `-z#####` flags (e.g. `-z56301`) from the command line to lookup specific ZIP code records.
- Includes error handling for malformed input and missing files.

## Files in the Project

- **`main.cpp`** – The main application logic, including command-line parsing and user interaction.
- **`CSVBuffer.h / CSVBuffer.cpp`** – Handles reading, indexing, converting, and summarizing ZIP code records.
- **`HeaderBuffer.h`** – Defines the structure and reading/writing of metadata headers for length-indicated files.
- **`us_postal_codes.csv`** – Sample CSV input file.
- **`ZipCodes`** – Sample converted length-indicated file (with header + records).
- **`README.md`** – This documentation.

## Compilation Instructions

Use the following command to compile:

```sh
g++ -o myProgram main.cpp CSVBuffer.cpp
```

This creates an executable named `myProgram`.

## Running the Program

### 1. Interactive Mode

Launch the program without flags:

```sh
./myProgram
```

You’ll be prompted to:
- Convert a CSV file to length-indicated format (Option 1), **or**
- Process a file and generate a summary table (Option 2)

### 2. ZIP Code Lookup via Command Line

You can directly lookup one or more ZIP codes using `-z#####` flags:

```sh
./myProgram ZipCodes -z56301 -z90210 -z99999
```

If a ZIP code is found, the full record will be printed with all field labels.  
If not, an appropriate message will be shown.

The file (`ZipCodes`) must be the converted length-indicated format.

## Output Example (ZIP Lookup)

```
Zip Code: 56301, Place Name: Saint Cloud, State: MN, County: Stearns, Latitude: 45.555, Longitude: -94.167
Zip Code 99999 not found in the file.
```

## Output Example (State Summary)

```
State, Easternmost, Westernmost, Northernmost, Southernmost
MN, 55001, 56763, 56701, 55044
TX, 78330, 79851, 79083, 78575
...
```

## Error Handling

- If input or output files can’t be opened, a descriptive error will be printed.
- Malformed rows are skipped gracefully.
- Invalid ZIP code flags will show a warning.

## Authors

Cha Vue, Sofia Hoffman, Alexander Miller, Zoljargal Enkhbayar, Yohannes Niguesse, Fatha Abdi

