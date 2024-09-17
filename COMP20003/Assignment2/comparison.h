/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    comparison.h :
            = the interface module for the comparison type of Assignment 2
--------------------------------------------------------------------------- */

// Guard
#ifndef _COMPARISON_H_
#define _COMPARISON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define BITS_PER_BYTE 8

typedef struct Comparison comparison_count_t;

void reset_comparison_count(comparison_count_t *counts);

comparison_count_t* create_comparison_count();

void free_comparison_count(void *ptr);

void increment_bit_comparisons(comparison_count_t *counts);

void increment_bit_comparisons_by_1(comparison_count_t *count);

void increment_node_accesses(comparison_count_t *counts);

void increment_string_comparisons(comparison_count_t *counts);

void print_comparison_count(comparison_count_t *count);

int compare_strings(char *str1, char *str2, int *bit_count);

int* get_bit_comparisons(comparison_count_t *count);

#endif
