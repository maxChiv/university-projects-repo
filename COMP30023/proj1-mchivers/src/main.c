/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    main.c :
            = the driver and entry point of the program
--------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "memory-manager.h"

#define MAX_LINE_LENGTH 12  /* Max Line Length for unsigned 32 bit int:
                             * 10 for digits, 1 for \n, 1 for \0 
                             */

int main(int argc, char *argv[]) {
    // Take in program arguments
    char *filename = NULL;
    int task = -1;

    for (int i = 1; i < argc; i++) {
        // Check after a flag
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            filename = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            // Assuming an input of {task1, task2, task3, task4}
            char *task_arg = argv[i+1];
            task = task_arg[strlen(task_arg) - 1] - '0';  // convert task number to integer
            i++;
        }
    }
    assert(filename != NULL && task != -1);

    // Open file and read in lines to memory-manager
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }
    
    // Create the memory manager
    memory_manager_t *mm = memory_manager_create_task_specific(task);

    // Read in a logical address call line by line
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        // Convert string to unsigned int
        uint32_t logical_address = (uint32_t) strtoul(line, NULL, 10);

        // TODO: Call memory-manager file's run(logical_address);
        handle_logical_address(mm, logical_address, task);
    }

    // Close file
    fclose(file);

    // Free the memory manager
    memory_manager_destroy(mm);

    return 0;
}
