/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    dict.h :
            = the interface module for the dictionary and operations
--------------------------------------------------------------------------- */

#ifndef _DICT_H_
#define _DICT_H_ 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linkedlist.h"
#include "data.h"
#include "io.h"
#include "comparison.h"

#define SEARCH_MODE 1
#define DELETE_MODE 2
#define COMPARE_MODE 3
#define RADIX_SEARCH_MODE 4
#define COMPARE 1  // Set to allow comparisons or not

typedef struct dict dict_t;

// Basic Functions
void free_dict(dict_t* d);
dict_t* build_dict(FILE *csv, int stage);
void print_dict(dict_t *dict, FILE *f);

// Stage Functions
void search_keys(dict_t *dict, char* keys[], int n_keys, FILE *f);
void delete_keys(dict_t *dict, char* keys[], int n_keys, FILE *f);

#endif

