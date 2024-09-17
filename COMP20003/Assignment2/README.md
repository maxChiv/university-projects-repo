
# COMP20003 Assignment 2 - Spellcheck Lookup

**Student Name:** Max Chivers 
**Student ID:** 1473341 
**Date:** 06/09/2024
**Grade:** [Grade]

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
8. [Repository Setup](#repository-setup)
9. [Acknowledgements](#acknowledgements)
10. [Full Task Description](#full-task-description)
11. [Unfinished / To do](#unfinished--to-do)

## Overview

This assignment involves creating two versions of a dictionary program. The first version (Stage 3) is a search-based, linked list dictionary, while the second version (Stage 4) uses a radix tree for optimized data management and retrieval. Both incorporate a comparison struct to count the number of comparisons made. The program reads data from CSV files, processes it, and generates outputs based on the input queries. The Makefile provides rules for compiling the code, running tests, and checking for memory leaks with Valgrind.

## Program Structure

- **Stage 3**: Implements a linked-list dictionary program to search for nodes, while counting comparisons.
- **Stage 4**: Implements a radix tree-based dictionary program, incorporating radix tree operations for optimized data retrieval, including a counting comparison.

## File Descriptions

- ```Complexity, Assignment 2 - COMP20003.pdf``` - the complexity report required to analyse the comparison of the two data structures for key based retrival. 

- ```driver.c``` - Contains the main driver logic for the program, including execution control.
- ```dict.c``` - Implements dictionary-related functions and data structures.
- ```io.c``` - Manages input and output operations, including file reading and writing.
- ```linkedlist.c``` - Provides functionalities for managing a linked list data structure.
- ```data.c``` - Handles data processing and manipulation.
- ```comparison.c``` - Contains functions for comparing data or elements.
- ```radix.c``` - Implements radix tree operations and functionalities.
- ```bitoperations.c``` - Provides utilities for bit-level operations.
- ```linkedlist.h``` - Header file for linkedlist.c, declaring linked list functions and structures.
- ```data.h``` - Header file for data.c, declaring data processing functions and structures.
- ```io.h``` - Header file for io.c, declaring I/O functions.
- ```comparison.h``` - Header file for comparison.c, declaring comparison functions.
- ```radix.h``` - Header file for radix.c, declaring radix tree functions and structures.
- ```bitoperations.h``` - Header file for bitoperations.c, declaring bit operation functions.
- ```tests/```- Directory containing test input files and expected output files.
- ```my_outputs/```- Directory containing output files generated from running tests.

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

To compile the programs, for Stage 3 and 4, use the provided `makefile`. The following commands will build the respective executables:

```bash
# Compile dict3 (Stage 3: Linked list search with comparison count)
make dict3
```
```bash
# Compile dict4 (Stage 4: Linked list search with comparison count)
make dict4
```

### Running the Programs

To run the programs for different stages, use the following commands:

```bash
# Run Stage 3 (Basic dictionary search with comparison count)
./dict3 3 <input_csv_file> my_outputs/<output_file> <input_query_file>

# Run Stage 3 (Radix trie dictionary search with comparison)
./dict4 4 <input_csv_file> my_outputs/<output_file> <input_query_file>
```

- `<input_csv_file>`: The CSV file containing suburb data (e.g., `tests/dataset_full.csv`).
- `<output_file>`: The file where the program writes results.
- `<input_query_file>`: A file containing queries to search or delete suburb entries.

## Example Executions

### Stage 3 Example

**Command:**

```bash
./dict3 3 tests/dataset_full.csv my_outputs/testfull.out < tests/testfull.in
```

**Expected Output to stdout:**

- See [matching results test1000.s3.stdout.out](matching_results/test1000.s3.stdout.out)

**Output to `[output_file]`:**
- See [matching results test1000.s3.out](matching_results/test1000.s3.out)

### Stage 4 Example

**Command:**
```bash
./dict4 4 tests/dataset_full.csv my_outputs/testfull.out < tests/testfull.in
```
**Expected Output to stdout:**

- See [matching results test1000.s4.stdout.out](matching_results/test1000.s4.stdout.out)

**Output to `[output_file]`:**
- See [matching results test1000.s4.out](matching_results/test1000.s4.out)

## Testing

To test the program, use the following make targets:

```bash
# Run tests
make test
```

The output will be stored in `[outputs/ directory]`, and `diff` files will be generated to compare actual results with the expected output.

## Cloning Specific Assignment 

To set up the project files, you can either clone the repository or download the files as a zip:

### Option 1: Cloning the Repository

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/maxChiv/university-projects-repo.git
   ```

2. **Navigate to the Assignment Folder:**

   ```bash
   cd university-projects-repo/COMP20003/Assignment2
   ```

   Replace `[repository_url]` with the URL of your university projects repository and `[assignment_folder]` with the specific folder for the assignment.

### Option 2: Downloading as a Zip

1. Navigate to the [repository URL](https://github.com/maxChiv/university-projects-repo.git).

2. Click on the **Code** button.

3. Select **Download ZIP** to download the entire repository.

4. Extract the downloaded ZIP file and navigate to `Assignment2`.

## Acknowledgements

- I would like to thank the course instructors for their guidance throughout the assignment.


## Full Task Description

**General**

You must read fully and carefully the assignment specification and instructions.

**Course:** COMP20003 Algorithms and Data Structures @ Semester 2, 2024

**Deadline Submission:** Friday 6th September 2024 @ 11:59 pm (end of Week 7)

**Course Weight:** 15%

**Assignment type:** individual

**ILOs covered:** 1, 2, 3, 4

**Submission method:** via ED

**Purpose**

The purpose of this assignment is for you to:

- Improve your proficiency in C programming and your dexterity with dynamic memory allocation.
- Demonstrate understanding of a concrete data structure (radix tree) and implement a set of algorithms.
- Practice multi-file programming and improve your proficiency in using UNIX utilities.
- Practice writing reports which systematically compare the efficiency of different data structures.

[Walkthrough](https://youtu.be/8pqSysBACnw)

**Your Task**

**Assignment:**

Overall, you will create a partial error-handling dictionary (spellchecker) using a radix tree.

You will be using the same dataset as Assignment 1.

Users will be able to query the radix tree and will get either the expected key, or the closest recommended key.

You will then write a report to analyze the time and memory complexity of your Assignment 1 linked list compared to your radix tree implementation.

**C Implementation:**

Your programs will build the dictionary by reading data from a file. They will insert each suburb into the dictionary (either the linked list (Stage 3) or radix tree (Stage 4)).

Your programs will handle the search for keys. There are three situations that your programs must handle:

1. **Handle exact matches:** Output all records that match the key (Stage 3 and 4).
2. **Handle similar matches:** If there are no exact matches, find the most similar key and output its associated records (Stage 4 only).
3. **Handle no matches being found:** If neither exact nor similar matches are found, indicate that there is no match or recommendation (Stage 3 and 4).

Your programs should also be able to populate and query the dictionary with the Assignment 1 linked list approach and the radix tree approach.

Your programs should also provide enough information to compare the efficiency of the linked list with the radix tree with an operation-based measure that ignores less important run-time factors (e.g., comparison count).

**An Introduction to Tries and Radix Trees**

First, it is important to establish the difference between a Trie and a Tree. This is best illustrated with an example. One example of a tree is a binary search tree (BST), where each node in the tree stores an entire string, as illustrated below. The nodes are ordered and allow easy searching. When searching in the BST, we compare the query with the entire string at each node, deciding whether to switch to the left or right subtree or stop (if the subtree is empty) based on the result of the comparison.

A trie is slightly different. It has multiple names, such as retrieval tree or prefix tree. In a trie, the traversal of the tree determines the corresponding key. For the same strings as above with one letter per node, it would look like:

Tries allow for quick lookup of strings. Querying this trie with the key "hey" would find no valid path after the "e" node. Therefore, you can determine that the key "hey" does not exist in this trie.

A radix trie is a subtype of trie, sometimes called a compressed trie. Radix trees do not store a single letter at each node but compress multiple letters into a single node to save space. At the character level, it would look like this:

Radix tries can again be broken down into different types depending on how many bits are used to define the branching factor. In this case, we are using a radix (r) of 2, which means every node has 2 children. This is accomplished by using 1 bit of precision, so each branch would be either a 0 or 1. This type of radix trie (with r=2) is called a Patricia trie. Another example of a radix trie with r=4 would have 4 children, determined by the binary numbers 00, 01, 10, 11. The radix is related to the binary precision by r = 2^x where x is the number of bits used for branching.

Radix trees benefit from less memory usage and quicker search times when compared to a regular trie.

While these visual representations are at the character level, a Patricia trie is implemented using the binary of each character. Each node in the trie stores the binary prefix at the current node, rather than the character prefix. For example, we can insert 3 binary numbers into a PATRICIA trie: 0000, 0010, and 0011.

Combining the previous worded example with the binary representation gives us a Patricia tree of the form:

You should trace along each path and validate that the stored strings are the same as the example above. Each character is represented over 8 bits, and the last character is followed by an end-of-string 00000000 8-bit character, i.e., NULL.

It is important to note that these representations only show the prefix at each node. An actual implementation will require more information within a node struct. To see this, look at the "extra insertion example" slide.

**Implementation Details**

Assignment 2 will involve roughly three new stages.

- **Stage 3** will extend your Assignment 1 solution to count the number of comparisons it takes to find a given key, i.e., a search query.

- **Stage 4** will implement the lookup of data by a given key (Official Name Suburb) in a Patricia tree.

- **Stage 5** is a report about the complexity of a linked list compared to the Patricia tree.

**Stage 3: Linked List Search Comparisons**

In this stage, you will extend your Assignment 1 solution to count the number of binary and string comparisons and node accesses used when searching for a given key.

Your Makefile will produce an executable called `dict3`. The command line arguments are identical to Assignment 1 but with the stage being 3 instead.

- The first argument will be the stage; for this part, the value will always be 3 (for linked list lookup with comparison count added).
- The second argument will be the name of the input CSV data file.
- The third argument will be the name of the output file.

Your `dict3` program should function exactly the same as Assignment 1's stage 1. You will add the functionality to count comparisons when searching for a key which will be added to the stdout output. For this stage, and this stage only, you may assume:

- Each character compared adds exactly 8 bits to the bit comparison count.
- The node access is incremented once per accessed linked list node.
- Each string comparison, even if a mismatch occurs on the first character, is 1 string comparison.

You should create the functionality to store comparisons with Stage 4 in mind. You will also be recording comparisons in the Patricia tree implementation, so your code should be easily applied to both. For this reason, you may want to extend your search function to include a pointer to a struct that holds information about the query and results.

**Important Notes:**

- You do not have to implement any spellchecking in the linked list. Your only task at this stage is to add functionality to count comparisons.
- You do not need to add this functionality to the deletion of nodes. Only the search will be assessed. You should, however, be able to recognize how to implement this in your deletion code.

**Example Execution:**

```bash
make -B dict3
./dict3 3 tests/dataset_1.csv output.txt < tests/test1.in
```

**Example Output**

The expected output to the output file would look like:

```
123
Carlton -->
COMP20003 Code: 9773, Official Code Suburb: 20495, Official Name Suburb: Carlton, Year: 2021, Official Code State: 2, Official Name State: Victoria, Official Code Local Government Area: 24600, Official Name Local Government Area: Melbourne, Latitude: -37.8004392, Longitude: 144.9680900
South Melbourne -->
```

With the following printed to stdout:

```
12
Carlton --> 1 records - comparisons: b64 n1 s1
South Melbourne --> NOTFOUND
```

**Note:** The bit comparisons are 8 times the character comparisons here.

**Stage 4: Patricia Tree Spellchecker**

In Stage 4, you will implement another dictionary allowing the lookup of data by key (Suburb Name).

Your Makefile should produce an executable program called `dict4`. This program should take three command line arguments:

- The first argument will be the stage; for this part, the value will always be 4 (for Patricia tree lookup and comparison counting).
- The second argument will be the name of the input CSV data file.
- The third argument will be the name of the output file.

Your `dict4` program should:

- Read the data in the same format as Assignment 1 and Stage 1, and save each entry in a Patricia tree using the Official Name Suburb as the key.
- Accept Official Name Suburbs from stdin, and search the tree for the matching key. Since your program should act as a simple spellchecker, an exact match may not be found. Follow the process "mismatch in key" as outlined below to implement spellchecking logic.
- Output all matching records to the output file, where a matching record may be an exact match or the closest match determined by the mismatch key algorithm. If no matches for the query are found (i.e., the trie is empty), your

## Unfinished / To do
- Implement the exact matching results instead of almost exactly correct lookup
   - clearly some sort of bug in creation of radix trie or implementation of search
- clearer commenting
- valgrind check for memory leaks
- create make targets for testing using diff 
- create make targets for valgrind and gdb