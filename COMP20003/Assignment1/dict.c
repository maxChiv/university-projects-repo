/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 1
    dict.c :
            = the implementation of module for dictionary operations
--------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dict.h"

struct dict {
    int mode;
    list_t *list;
};

/* Frees the dictionary of its list */
void free_dict(dict_t* d) {
    if(d) {
        free_list(d->list, free_suburb);
        free(d);
    }
}

/* Builds the corresponding dict based on a csv file */
dict_t* build_dict(FILE *csv, int stage) {
    assert(csv);

    dict_t *d = (dict_t*)malloc(sizeof(dict_t));
    assert(d);
    list_t *l = d->list = make_empty_list();
    d->mode = stage;

    char* row;
    while((row = get_csv_row(csv)) != NULL) {
        append(l, parseRowToSuburb(row));
        free(row);
    }
    return d;
}


/* Prints the dict in CSV style to output 
    - Assumes it is the list wiht data implmented from data.h */
void print_dict(dict_t *dict, FILE *f) {
    assert(dict && dict->list);
    print_headers(f);
    print_list_data(f, dict->list, print_suburb);
}

/* Performs the appropriate operations for searching the dict through some keys
    - Assumes it is the list wiht data implmented from data.h */
void search_keys(dict_t *dict, char* keys[], int n_keys, FILE *f) {
    assert(dict && dict->list);
    // Iterate over each key to search 
    list_t *search_result;
    for(int i = 0; i < n_keys; i++) {
        search_result = search_all(dict->list, keys[i], cmpSuburbName, cpySuburb);
        output_search_result(keys[i], search_result, print_with_headers, f);
        free_list(search_result, free_suburb);
    }
}

/* Performs the appropriate operations for deleting in the dict through some keys
    - Assumes it is the list wiht data implmented from data.h */
void delete_keys(dict_t *dict, char* keys[], int n_keys, FILE *f) {
    assert(dict && dict->list);
    // Iterate over each key to search 
    int n_deleted = 0;
    for(int i = 0; i < n_keys; i++) {
        n_deleted = rmv_all(dict->list, keys[i], cmpSuburbName, free_suburb);
        stdout_deletion(keys[i], n_deleted);
    }
    print_dict(dict, f);
}

