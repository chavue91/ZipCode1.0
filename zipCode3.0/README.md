# ZipCode3.0  
## ZIP Code Blocked Sequence Set Processor

## Description

`ZipCode3.0` is a C++ application that processes U.S. ZIP code data in a structured blocked sequence set format. The application supports:

- Converting CSV data into a **length-indicated** format with metadata.
- Converting that format into a **blocked sequence set file (.bss)**.
- Creating a **primary key index** of high keys per block.
- Searching for ZIP code records using an efficient block index.
- Dynamically **inserting** or **deleting** ZIP code records with logging and index updates.
- Dumping the blocked file **by physical** or **logical order**.

## Features

- Converts CSV → Length-Indicated → Blocked Sequence Set (`.bss`)
- Generates and uses a **simple key-to-RBN index file** (`.idx`)
- Efficient in-place record **insertion** and **deletion**
- Includes structured **header metadata**, **record sorting**, and **record splitting**
- Supports **sequential dumps** of blocks in physical or logical order
- Fast **primary key search** using `-z#####` flags
- Log messages for **splits, merges, index rewrites**, and **failures**

## Project Structure

| File               | Description                                                  |
|--------------------|--------------------------------------------------------------|
| `main.cpp`         | Entry point for CSV processing, summary generation, or search |
| `CSVBuffer.cpp/h`  | Converts, loads, searches, and summarizes ZIP code records    |
| `HeaderBuffer.h`   | Defines headers for length-indicated format                   |
| `HeaderBuffer3.cpp/h` | Defines and manages blocked sequence set headers             |
| `BlockBuffer.cpp/h` | Reads and writes blocks from/to `.bss` files                  |
| `RecordBuffer.cpp/h`| Unpacks record strings to extract fields or keys             |
| `ConvertToBlocked.cpp` | Converts `.txt` to `.bss` with structured block layout       |
| `BuildIndex.cpp`   | Generates `key → RBN` index from a `.bss` file                |
| `InsertRecord.cpp` | Adds records to a `.bss` file and updates the index           |
| `DeleteRecord.cpp` | Removes records and updates metadata/index                    |
| `DumpUtility.cpp/h`| Dumps `.bss` file in physical or logical order                |
| `SearchIndex.cpp`  | Looks up ZIP records from a `.bss` file via `.idx`            |

## Build Instructions

Run:

```sh
make
```

This compiles:

- `myProgram` – main UI for conversion and summary
- `ConvertToBlocked` – convert `.txt` to `.bss`
- `BuildIndex` – generate `.idx` from `.bss`
- `InsertRecord`, `DeleteRecord` – add/remove records
- All shared `.o` dependencies

## Step-by-Step Usage

### 1. Convert CSV to Length-Indicated

```sh
./myProgram
# Choose option 1
# Provide CSV file (e.g. us_postal_codes.csv)
# Output: zipCodes.txt
```

### 2. Convert to Blocked Sequence Set

```sh
./ConvertToBlocked zipCodes.txt zipCodes.bss
```

### 3. Build Primary Key Index

```sh
./BuildIndex zipCodes.bss zipCodes.idx
```

### 4. Dump the File

```sh
./myProgram zipCodes.bss --dump-physical
./myProgram zipCodes.bss --dump-logical
```

### 5. Search by ZIP Code

```sh
./myProgram zipCodes.bss -z56301 -z90210 -z00000
```

### 6. Insert Records

```sh
./InsertRecord zipCodes.bss zipCodes.idx new_zips.csv
```

### 7. Delete Records

```sh
./DeleteRecord zipCodes.bss zipCodes.idx to_delete.txt
```

## Output Example (Search)

```
Found: 56301,Saint Cloud,MN,Stearns,45.555,-94.167
Zip Code 00000 not found in data block.
```

## Output Example (Dump)

```
List Head:  0
Avail Head: -1
0  10001 10002 10003  1
1  10004 10005       -1
```

## Error Handling

- Skips malformed lines
- Handles corrupted index or `.bss` files
- Detects overfilled blocks before writing
- Logs when splitting, merging, or updating header/index fails

## Authors

Cha Vue, Sofia Hoffman, Zoljargal Enkhbayar, Yohannes Niguesse

