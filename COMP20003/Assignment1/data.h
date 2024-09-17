/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 1
    data.h :
            = the interface module for the data type of Assignment 1
--------------------------------------------------------------------------- */

// Guard
#ifndef _DATA_H_
#define _DATA_H_ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_HEADERS 10

typedef struct Suburb suburb_t;

// Printing Functions
void print_headers(FILE* f);
void print_with_headers(FILE* f, void *suburb_void);
void print_suburb(FILE* f, void *suburb_void);
void print_suburb_component(FILE* f, suburb_t *suburb, int index);
void print_suburb_component_csv(FILE* f, suburb_t *suburb, int index);

// Header function 
void determine_headers(char *header_line);

// Comparing function
int cmpSuburbName(void *a, void *b);

// Freeing Function
void free_suburb(void *suburb);
void free_headers(void);

suburb_t *parseRowToSuburb(char *row);
void *cpySuburb(void *s);

#endif
