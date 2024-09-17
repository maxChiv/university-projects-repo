/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 1
    io.c :
            = the implementation of module for input and output specified
--------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "io.h"
#include "linkedlist.h"

/* Retrieves a single line from the csv file provided and returns NULL if no
line was read or the line read.
    - Assumes f is a csv file */
char* get_csv_row(FILE *f) {
    assert(f);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Read a single line from the file
    if ((read = getline(&line, &len, f)) != -1) {
        return line;  // Return the line read
    }

    // If no line is read (end of file or error), free the allocated memory and return NULL
    free(line);
    return NULL;
}

/* Outputs the appropriate format for a search result of a single key */
void output_search_result(char* key, list_t *results, PrintFunc print_data, 
    FILE *output_file) {
    // To standard output 
    int n_results = get_list_size(results);
    
    printf("%s %s ", key, ARROW_STR);

    if (n_results > 0) {
        printf("%d records found\n", n_results);
    } else {
        printf("%s\n", NOT_FOUND_STR);
    }

    // To output file
    fprintf(output_file, "%s %s\n", key, ARROW_STR);
    print_list_data(output_file, results, print_data);
}

/* Outputs the appropriate format after a deletion result of a single key for the
standard output */
void stdout_deletion(char* key, int n_removed) {
    printf("%s %s ", key, ARROW_STR);
    if (n_removed > 0) {
        printf("%d records deleted\n", n_removed);
    } else {
        printf("%s\n", NOT_FOUND_STR);
    }
}

