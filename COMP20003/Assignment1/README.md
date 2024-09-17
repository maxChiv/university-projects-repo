
# COMP20003 Assignment 1 - Dictionary Implementation Using Linked Lists

**Student Name:** Max Chivers 
**Student ID:** 1473341
**Date:** 16/08/2024
**Grade:** 100% 

## Table of Contents

1. [Overview](#overview)
2. [Program Structure](#program-structure)
3. [File Descriptions](#file-descriptions)
4. [Dataset](#dataset)
5. [Instructions](#instructions)
   - [Compiling](#compiling)
   - [Running the Programs](#running-the-programs)
6. [Example Executions](#example-executions)
7. [Testing](#testing)
8. [Memory Debugging and Profiling](#memory-debugging-and-profiling)
9. [Cloning Assignment](#cloning-assignment)
10. [Acknowledgements](#acknowledgements)
11. [Full Task Description](#full-task-description)

## Overview

This assignment implements a simple dictionary structure using linked lists. The program allows users to search and manipulate suburb data stored in a CSV format. The assignment is split into two stages:

- **Stage 1:** Basic dictionary with search functionality using linked lists.
- **Stage 2:** Extended functionality to handle deletions from the dictionary.

## Program Structure

- **Stage 1:** Involves reading the full dataset of suburbs from a CSV file and allowing users to search the dictionary.
- **Stage 2:** Adds functionality to delete suburb records from the dictionary as specified by the input.

Both stages utilize the linked list data structure, implemented in C, to store and manipulate suburb information.

## File Descriptions

- `driver.c` - Main file that handles input/output and drives the program based on the stage specified.
- `dict.c` - Core dictionary functionality for managing suburb data, including search and (for Stage 2) deletion.
- `io.c` - Handles reading from input files and writing output results specific to requirements.
- `linkedlist.c` - Implements the linked list data structure for storing suburb data.
- `data.c` - Manages the suburb data and its related operations.
- `dict.h` - Header file for the dictionary-related functions.
- `linkedlist.h` - Header file for the linked list data structure.
- `data.h` - Header file for suburb data operations.
- `tests/` - Contains input files for testing the program.
- `my_outputs/` - Contains the generated output files during test runs.

## Dataset
The [opendatasoft](https://data.opendatasoft.com/pages/home/) website provides numerous databases, enabling organisations and individuals to seamlessly access and share data. The dataset used in this assignment is a simplified version derived from the ["State Suburbs - Australia"](https://data.opendatasoft.com/explore/dataset/georef-australia-state-suburb%40public/information/?disjunctive.ste_code&disjunctive.ste_name&disjunctive.lga_code&disjunctive.lga_name&disjunctive.scc_code&disjunctive.scc_name) database on that website which originates from ABS data. 
**The structure of the data is given below:**
```
COMP20003 Code                      - The record IDs added by the COMP20003 teaching team (integer)
Official Code Suburb                - The suburb ID (integer)
Official Name Suburb                - The name of the suburb (string)
Year                                - The year the information was recorded for (integer)
Official Code State                 - The IDs of the corresponding states (string)
Official Name State                 - The names of the corresponding states (string)
Official Code Local Government Area - The IDs of the corresponding local government areas (string)
Official Name Local Government Area - The names of the corresponding local government areas (string)
Latitude                            - The latitude (y) of the suburb centre (double)
Longitude                           - The longitude (x) of the suburb centre (double)
```


## Instructions

### Compiling

To compile the programs for Stage 1 and Stage 2, use the provided `makefile`. The following commands will build the respective executables:

```bash
# Compile dict1 (Stage 1: Basic search functionality)
make dict1

# Compile dict2 (Stage 2: Search and deletion functionality)
make dict2
```

### Running the Programs

To run the programs for different stages, use the following commands:

```bash
# Run Stage 1 (Basic dictionary search)
./dict1 1 <input_csv_file> my_outputs/<output_file> <input_query_file>

# Run Stage 2 (Dictionary with deletion functionality)
./dict2 2 <input_csv_file> my_outputs/<output_file> <input_query_file>
```

- `<input_csv_file>`: The CSV file containing suburb data (e.g., `tests/dataset_full.csv`).
- `<output_file>`: The file where the program writes results.
- `<input_query_file>`: A file containing queries to search or delete suburb entries.

## Example Executions

### Stage 1 (Basic Dictionary Search)

**Command:**

```bash
./dict1 1 tests/dataset_full.csv my_outputs/testfull.out < tests/testfull.in
```

**Expected Output to stdout:**

- See [matching results testfull.stdout.out](matching_results/testfull.stdout.out)

**Output to `my_outputs/testfull.out`:**

- See [matching results testfull.out](matching_results/testfull.out)

### Stage 2 (Dictionary with Deletion)

**Command:**

```bash
./dict2 2 tests/dataset_del.csv my_outputs/delfull.out < tests/del3.in
```

**Expected Output to stdout:**


- See [matching results del3.stdout.out](matching_results/del3.stdout.out)

**Output to `my_outputs/delfull.out`:**

- See [matching results del3.out](matching_results/del3.out)

## Testing

To test the program against provided datasets and queries, the following make targets can be used:

```bash
# Test Stage 1 (search functionality)
make test_stage1

# Test Stage 2 (search and delete functionality)
make test_stage2
```

The output will be stored in `my_outputs/`, and `diff` files will be generated to compare the actual results with the expected output.

## Memory Debugging and Profiling

### Using Valgrind

Valgrind can be used to check for memory leaks or other memory-related issues. The following make targets are available:

```bash
# Run Valgrind for Stage 1
make valgrind_dict1

# Run Valgrind for Stage 2
make valgrind_dict2
```

Valgrind will perform a thorough memory analysis, reporting any memory leaks, invalid memory accesses, or other issues.

### Using GDB

To run the program using GDB for debugging:

```bash
# Run GDB for Stage 1
make gdb_dict1

# Run GDB for Stage 2
make gdb_dict2
```

## Cloning Assignment 

To set up the project files, you can either clone the repository or download the files as a zip:

### Option 1: Cloning the Repository

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/maxChiv/university-projects-repo.git
   ```

2. **Navigate to the Assignment Folder:**

   ```bash
   cd university-projects-repo/COMP20003/Assignment1
   ```

### Option 2: Downloading as a Zip

1. Navigate to the [repository URL](https://github.com/maxChiv/university-projects-repo.git).

2. Click on the **Code** button.

3. Select **Download ZIP** to download the entire repository.

4. Extract the downloaded ZIP file and navigate to `COMP20003/Assignment1`.

## Acknowledgements

- I would like to thank the course instructors for their guidance throughout the assignment.

## Full Task Description

### General Information

- **Course:** COMP20003 Algorithms and Data Structures @ Semester 2, 2024
- **Deadline Submission:** Friday 16th August 2024 @ 5 pm (end of Week 4)
- **Course Weight:** 10%
- **Assignment Type:** Groups of up to 2
- **ILOs Covered:** 2, 3, 4
- **Submission Method:** via ED

### Purpose

The purpose of this assignment is to:

- Improve your proficiency in C programming and dynamic memory allocation.
- Demonstrate your understanding of a concrete data structure, specifically linked lists.
- Practice multi-file programming and enhance your skills with UNIX utilities.

For a walkthrough of the assignment, please refer to the following video: [Assignment Walkthrough](https://www.youtube.com/watch?v=ha9lBHWnt2E).

### Your Task

#### Assignment Overview

In this assignment, you will:

- Create a simple dictionary based on a linked list to store information about Australian suburbs from a dataset.
- Implement functionalities to:
  - Search for suburbs using attributes (keys) in the dataset.
  - Remove specific suburbs from the dictionary.

#### C Implementation

Your program will:

- Build the dictionary by reading data from a file and inserting each suburb record as a node in a linked list.
- Include the following tools:
  - **Search Tool:** Looks for a key in the list and outputs matching records.
  - **Removal Tool:** Removes records associated with specified keys and outputs the remaining records to a new CSV file.

### Implementation Details
Assignment 1 involves producing two programs, classified into two stages:

- **Stage 1:** Implements the lookup of data by a given key (Official Name Suburb).
- **Stage 2:** Implements the deletion of data by a given key (Official Name Suburb), with the remaining data output after all deletions.

While Stage 1 and Stage 2 programs can be the same, you must produce both from your Makefile. In Assignment 2, you will build upon these stages with additional functionalities using a new data structure that utilizes the dictionary from both stages.

**Stage 1 - Data Retrieval**

In Stage 1, your task is to implement a dictionary allowing data lookup by key (Suburb Name).

**Program Specifications**

- **Executable Name:** `dict1`
- **Command Line Arguments:**
  1. **Stage Number:** Always `1` for lookup.
  2. **Input File:** Name of the input CSV data file.
  3. **Output File:** Name of the output file.

**Program Functionality**

1. **Data Reading:**
   - Read data from the specified CSV file.
   - Store data in a linked list with each record (row) as a separate node in the list.
   - Data types should match those in the Dataset and Assumptions slide.
   - The Official Name Suburb is assumed to be the second column (0-based index) in the dataset.

2. **Search Operation:**
   - Accept Official Name Suburbs from stdin and search the linked list for matching records.
   - Output matching records to the output file.
   - If no matches are found, output `NOTFOUND` to the output file.
   - Output the number of records found to stdout.

3. **Modularity Considerations:**
   - Store search results independently from the list structure to facilitate future modifications.
   - Structure your code to allow passing processed data to other functions for constructing new data structures.

4. **Testing:**
   - Create a file with keys to be searched, one per line, and redirect the input from this file using the UNIX operator `<`.

**Example Execution**

1. **Compiling:**
   ```bash
   make -B dict1
   ```
2. **Running the Program:**
   ```bash
   ./dict1 1 tests/dataset_100.csv output.txt
   ```

   **Input Queries:**
   ```
   Melbourne
   Carlton
   Far Far Away
   ```

   - **Line 1:** Compiles the program.
   - **Line 2:** Runs the program specifying Stage 1 (lookup). Searches the dataset `tests/dataset_100.csv` and writes results to `output.txt`.
   - **Lines 3-5:** Provide search queries.

**Example Output**

**Output File (`output.txt`):**
```
1234567
Melbourne -->
COMP20003 Code: 6623, Official Code Suburb: 21640, Official Name Suburb: Melbourne, Year: 2021, Official Code State: 2, Official Name State: Victoria, Official Code Local Government Area: 24600, 25900, 26350, 27350, Official Name Local Government Area: Melbourne, Port Phillip, Stonnington, Yarra, Latitude: -37.8249613, Longitude: 144.9715278
Carlton -->
COMP20003 Code: 9773, Official Code Suburb: 20495, Official Name Suburb: Carlton, Year: 2021, Official Code State: 2, Official Nam
```

**Output to `stdout`:**
```
123
Far Far Away --> NOTFOUND
```

### Implementation Requirements

To meet the assignment requirements, ensure the following:

- **Programming Language:** Code must be written in C.
- **Record Structure:** Use a custom `struct` for each data record representing a suburb, with appropriate member variables (e.g., Year, Suburb Code) and data types (int, double, string).
- **Linked List Implementation:** Use a linked list to preserve the order of data records from the input file.
- **Modular Design:** Separate your code into distinct modules. Dictionary operations should be in their own `.c` and `.h` files, separate from the main program. This modular approach should facilitate easier use in other programs.
- **Multiple Dictionary Support:** Design functions to be easily extensible for handling multiple dictionaries. Functions should include arguments for the dictionary pointer (e.g., `search(dictionary, value)`).
- **Single File Read:** Read the input file only once.
- **Space-Efficient Strings:** Allocate space for strings efficiently, including space for the end-of-string character (`\0`).
- **Exact Output Matching:** Ensure your outputs match the expected results exactly, including the order of data. Outputs must be character-for-character identical to the provided test cases.
- **Makefile:** Provide a Makefile to direct the compilation of your program. Ensure the Makefile is in the same directory as your code. Use `make dict1` to build the first program and `make dict2` for the second. The Makefile must be submitted with your assignment.

---