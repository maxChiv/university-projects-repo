/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    driver.c :
            = the main entrance for the program 
--------------------------------------------------------------------------- */

#include <stdlib.h>
#include "dict.h"
#include "linkedlist.h"
#include "data.h"
#include "io.h"
#include "comparison.h"

#define INITIAL_SIZE_KEYS 3 

void input_args_config(int *mode, int argc, const char** argv, 
                char** input_file, char** output_file);
char** get_keys(int *n_keys);


int main(int argc, char const *argv[]) {
    int stage;
    char *inputf_name, *outputf_name;
    input_args_config(&stage, argc, argv, &inputf_name, &outputf_name);

    FILE *inputf = fopen(inputf_name, "r"), *outputf = fopen(outputf_name, "w");

    char* header_line = get_csv_row(inputf);
    determine_headers(header_line);
    free(header_line);
    dict_t *dict = build_dict(inputf, stage);

    int n_keys;
    char** keys = get_keys(&n_keys);

    switch (stage) {
        case SEARCH_MODE:
            search_keys(dict, keys, n_keys, outputf);
            break;
        case DELETE_MODE:
            delete_keys(dict, keys, n_keys, outputf);
            break;
        case COMPARE_MODE:
            search_keys(dict, keys, n_keys, outputf);
            break;
        case RADIX_SEARCH_MODE:
            search_keys(dict, keys, n_keys, outputf);
            
            break;
    }
    

    // Free everything used
    for (int i = 0; i < n_keys; i++) {
        free(keys[i]);  // Free each string
    }
    free(keys);  // Free the array of strings
    free_headers();  // Frees data.c header of csv array 
    free_dict(dict);
    free(inputf_name);
    free(outputf_name);

    // Close files
    fclose(inputf);
    fclose(outputf);
}

/* This takes the input arguments and configures the mode int pointer, and the two
file pointers to match the appropriate input arguments:
    1. The first argument will be the *stage*, for this part, the value will 
        always be `1` (for lookup).
    2. The second argument will be the name of the input *CSV* *data file*.
    3. The third argument will be the name of the *output* *file*.
*/
void input_args_config(int *mode, int argc, const char** argv, 
                char** input_file, char** output_file) {
    assert(argc > 3);
    // argv[1] is stage
    *mode = atoi(argv[1]);

    // argv[2] is input_file (for csv)
    *input_file = strdup(argv[2]);

    // argv[3] is output_file 
    *output_file = strdup(argv[3]);
}

/* Retrieves all the keys from the stdin */
char** get_keys(int *n_keys) {
    char **lines = malloc(INITIAL_SIZE_KEYS * sizeof(char*)); // Dynamic array of strings
    assert(lines);

    int size = INITIAL_SIZE_KEYS; // Current allocated size of the array
    int count = 0;                // Number of lines read
    char *line = NULL;            // Pointer to hold each line
    size_t len = 0;               // Variable to store the length of the line

    // Read lines from stdin until EOF
    while (getline(&line, &len, stdin) != -1) {
        // If the array is full, resize it
        if (count >= size) {
            size *= 2; // Double the size
            char **tmp = realloc(lines, size * sizeof(char*));
            assert(tmp);
            lines = tmp;
        }

        // Remove the newline character at the end of the line, if present
        line[strcspn(line, "\n")] = '\0'; // Replace newline with null terminator

        // Duplicate the line and store it in the array
        lines[count] = strdup(line);
        assert(lines[count]);
        count++;
    }
    free(line);
    *n_keys = count;
    return lines;
}
