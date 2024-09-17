/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    comparison.c :
            = the implementation of comparison module for Assignment 2
--------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "comparison.h"

struct Comparison {
    int bit_comparisons;
    int node_accesses;
    int string_comparisons;
};

/* Resets the count of the comparison */
void reset_comparison_count(comparison_count_t *counts) {
    counts->bit_comparisons = 0;
    counts->node_accesses = 0;
    counts->string_comparisons = 0;
}

/* Creates a  new empty comparison count */
comparison_count_t* create_comparison_count() {
    // Allocate memory for a new comparison_count_t structure
    comparison_count_t *new_count = (comparison_count_t *)malloc(sizeof(comparison_count_t));
    assert(new_count);
    
    // Initialize the fields of the structure
    reset_comparison_count(new_count);
    
    // Return the pointer to the newly created and initialized structure
    return new_count;
}

/* Function to free memory allocated for a comparison_count_t structure */
void free_comparison_count(void *ptr) {
    // Cast the void pointer to a comparison_count_t pointer
    comparison_count_t *count = (comparison_count_t *)ptr;
    
    // Free the allocated memory
    free(count);
}

/* Function to increment the bit comparisons count */
void increment_bit_comparisons(comparison_count_t *count) {
    if (count) {
        count->bit_comparisons += BITS_PER_BYTE;
    }
}

/* Function to increment the bit comparisons count */
void increment_bit_comparisons_by_1(comparison_count_t *count) {
    if (count) {
        count->bit_comparisons++;
    }
}

/* Function to increment the node accesses count */
void increment_node_accesses(comparison_count_t *count) {
    if (count) {
        count->node_accesses++;
    }
}

/* Function to increment the string comparisons count */
void increment_string_comparisons(comparison_count_t *count) {
    if (count) {
        count->string_comparisons++;
    }
}

/* Function to print comparison */
void print_comparison_count(comparison_count_t *count) {
    if (count) {
    // Print the comparison counts in the specified format
        printf(" - comparisons: b%d n%d s%d\n", 
                count->bit_comparisons, 
               count->node_accesses, 
               count->string_comparisons);
    }
}

/* Comparison function that also counts bit comparisons */
int compare_strings(char *str1, char *str2, int *bit_count) {
    while (*str1 && *str2) {
        unsigned char c1 = (unsigned char)*str1;
        unsigned char c2 = (unsigned char)*str2;
        
        // Count the entire character (8 bits) for each character comparison
        (*bit_count) += BITS_PER_BYTE;

        // Check if the characters are different
        if (c1 != c2) {
            return (c1 - c2); // Return the difference in ASCII values
        }
        
        // Move to the next characters
        str1++;
        str2++;
    }
    // For last byte/char comparison 
    (*bit_count) += BITS_PER_BYTE;

    // If we reached here, the strings are equal up to the shorter length
    return (*str1 - *str2);
}

/* Function to get the address of the bit_comparisons member */
int* get_bit_comparisons(comparison_count_t *count) {
    if (count == NULL) {
        return NULL; // Handle null pointer case
    }
    return &(count->bit_comparisons);
}
