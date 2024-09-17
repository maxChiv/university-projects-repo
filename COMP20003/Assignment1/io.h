/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 1
    io.h :
            = the interface module for the basic input and output for Assignment 1
--------------------------------------------------------------------------- */

#ifndef _IO_H_
#define _IO_H_ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "linkedlist.h"

#define MAX_CSV_LINE_LEN 511
#define NOT_FOUND_STR "NOTFOUND"
#define ARROW_STR "-->"

char* get_csv_row(FILE *f);

void output_search_result(char* key, list_t *results, PrintFunc print_data, 
    FILE *output_file);

void stdout_deletion(char* key, int n_removed);

#endif

