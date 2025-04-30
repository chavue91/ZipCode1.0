# ZipCode2.0 with B+Tree Support
# Zip Code Data Processor with Index and B+Tree Support

## Description

This program processes U.S. ZIP code data from a CSV or length-indicated file and supports:
- Generating a state-wise summary of extreme ZIP codes
- Searching ZIP codes using a fast in-memory primary key index via command-line flags
- Creating, storing, and manipulating data using a file-based B+Tree structure
- Visualizing the B+Tree organization
- Searching and retrieving records from the B+Tree

## Features

- Reads ZIP code data from a standard CSV or length-indicated format
- Converts CSV to a length-indicated file with a structured header
- Extracts fields such as zip code, city, state, latitude, and longitude
- Generates a summary of easternmost, westernmost, northernmost, and southernmost ZIP codes per state
- Creates and uses a **primary key index** in RAM for fast ZIP code lookups
- Implements a B+Tree file organization following Folk Section 9.10 specifications
- Supports both index blocks and sequence blocks in the same file
- Implements doubly-linked sequence sets for efficient sequential access
- Provides visualization tools for the B+Tree structure
- Supports creating, inserting, deleting, and traversing B+Tree records
- Enables searching for records by ZIP code or browsing records sequentially
- Accepts `-z#####` flags (e.g. `-z56301`) from the command line to lookup specific ZIP code records
- Includes error handling for malformed input and missing files

## Files in the Project

- **`main.cpp`** – The main application logic, including command-line parsing and user interaction
- **`CSVBuffer.h / CSVBuffer.cpp`** – Handles reading, indexing, converting, and summarizing ZIP code records
- **`HeaderBuffer.h`** – Defines the structure and reading/writing of metadata headers for length-indicated files
- **`BPlusTree.h / BPlusTree.cpp`** – Implements the B+Tree file organization with both index and sequence blocks
- **`us_postal_codes.csv`** – Sample CSV input file
- **`ZipCodes`** – Sample converted length-indicated file (with header + records)
- **`README.md`** – This documentation

## B+Tree Implementation

The B+Tree implementation follows these key design principles:
- **File-Based Storage**: The entire B+Tree structure resides in a single file
- **Minimal RAM Usage**: Only the header and currently accessed blocks are kept in memory
- **Fixed-Size Blocks**: All blocks (index and sequence) have the same size
- **Doubly-Linked Sequence Set**: Leaf blocks form a linked list for sequential access
- **Index Blocks**: Store ordered pairs of keys and block pointers
- **Sequence Blocks**: Store the actual ZIP code records

## Compilation Instructions

Use the following command to compile:

```sh
make
```

This creates an executable named `zipcode`.

## Running the Program

### 1. Interactive Mode

Launch the program without flags:

```sh
./zipcode
```

You'll be prompted to choose from the following operations:
1. Convert CSV to length-indicated file
2. Process file and generate summary
3. Print B+Tree structure
4. Insert a record into B+Tree
5. Delete a record from B+Tree
6. Create new B+Tree from CSV
7. Search record in B+Tree

### 2. ZIP Code Lookup via Command Line

You can directly lookup one or more ZIP codes using `-z` flags:

```sh
./zipcode ZipCodes.dat -z90210 -z10001
```

This will:
1. Load the specified file
2. Build an in-memory index
3. Display the full record for each ZIP code (if found)

## B+Tree Operations

### Creating a B+Tree (Option 6)

To create a new B+Tree from a CSV file:
1. Run the program and select option 6
2. Enter the input CSV filename (e.g., `us_postal_codes.csv`)
3. Enter the output B+Tree filename (e.g., `zipcodes.btree`)
4. Specify the B+Tree parameters:
   - Order (recommended: 10) - Maximum number of children per node
   - Records per sequence block (recommended: 20) - Maximum records per leaf block
5. The program will create the B+Tree file and load all records from the CSV
6. You can choose to print the tree structure for verification

### Viewing B+Tree Structure (Option 3)

To visualize the structure of a B+Tree file:
1. Run the program and select option 3
2. Enter the name of the B+Tree file to view
3. The program will display:
   - General statistics about the tree (block count, height)
   - Level-by-level breakdown of the tree structure
   - Sequence set (leaf blocks) with their contents

### Searching in B+Tree (Option 7)

The program offers two ways to search for records in a B+Tree:

#### Search by ZIP Code
1. Run the program and select option 7
2. Enter the B+Tree filename
3. Choose search method 1 (Search by ZIP code)
4. Enter the ZIP code to search for
5. If found, the program displays the complete record details

#### Browse Records
1. Run the program and select option 7
2. Enter the B+Tree filename
3. Choose search method 2 (Display first N records)
4. Enter the number of records to display
5. The program shows the specified number of records in order

### Inserting Records (Option 4)

To insert a new record into an existing B+Tree:
1. Run the program and select option 4
2. Enter the B+Tree filename
3. Enter the record details:
   - ZIP code (e.g., `12345`)
   - Place name (e.g., `New City`)
   - State (two-letter code, e.g., `CA`)
   - County (e.g., `Example County`)
   - Latitude (e.g., `34.5678`)
   - Longitude (e.g., `-118.1234`)
4. The program will insert the record and report success or failure
5. You can choose to print the tree structure to verify the insertion

### Deleting Records (Option 5)

To delete a record from a B+Tree:
1. Run the program and select option 5
2. Enter the B+Tree filename
3. Enter the ZIP code to delete
4. The program will:
   - Search for the record and display it if found
   - Ask for confirmation before deletion
   - Delete the record and report success or failure
   - Offer to print the updated tree structure

## Output Examples

### ZIP Lookup Output

```
Zip Code: 90210, Place Name: Beverly Hills, State: CA, County: Los Angeles, Latitude: 34.0901, Longitude: -118.4065
Zip Code: 10001, Place Name: New York, State: NY, County: New York, Latitude: 40.7500, Longitude: -73.9967
```

### B+Tree Search Output

```
Record found:
--------------------------------------------------
ZIP Code: 90210
Place Name: Beverly Hills
State: CA
County: Los Angeles
Latitude: 34.090149
Longitude: -118.406530
```

### B+Tree Structure Output

```
B+Tree Structure:
Total blocks: 42
Root block: 40
First leaf: 2
Last leaf: 39
Tree height: 3

Level 0: [40:I(2)] 
Level 1: [38:I(5)] [39:I(4)] 
Level 2: [2:S(10)] [3:S(10)] [4:S(10)] ...

Sequence set (linked list of leaf blocks):
Block 2 (prev=-1, next=3): 10001 10002 10003 ...
Block 3 (prev=2, next=4): 19901 19902 19903 ...
...
```

## Performance Considerations

- **Block Size**: Larger blocks reduce tree height but increase I/O cost per operation
- **Order**: Higher order reduces tree height but increases block size
- **Records Per Block**: More records per block reduce the number of blocks but increase block size
- **File Organization**: The B+Tree provides efficient random access and sequential traversal

## Error Handling

- If input or output files can't be opened, a descriptive error will be printed
- Malformed rows are skipped gracefully
- Invalid ZIP code flags will show a warning
- Failed B+Tree operations will report the reason for failure

## Authors

Cha Vue, Sofia Hoffman, Yohannes Niguesse