/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    dict.c :
            = the implementation of module for dictionary operations
--------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dict.h"
#include "comparison.h"
#include "radix.h"


struct dict {
    int mode;
    list_t *list;
    radix_node_t *radix_root;
};

/* Frees the dictionary of its list */
void free_dict(dict_t* d) {
    if(d) {
        if(d->list) {
            free_list(d->list, free_suburb);
        }
        if(d->radix_root) {
            freeRadixNode(d->radix_root, free_suburb);
        }

        free(d);
    }
}

/* Builds the corresponding dict based on a csv file */
dict_t* build_dict(FILE *csv, int stage) {
    assert(csv);

    dict_t *d = (dict_t*)malloc(sizeof(dict_t));
    assert(d);

    d->mode = stage;
    if (stage != RADIX_SEARCH_MODE) {
        list_t *l = d->list = make_empty_list();
        d->radix_root = NULL;
        char* row;
        while((row = get_csv_row(csv)) != NULL) {
            append(l, parseRowToSuburb(row));
            free(row);
        }
    } else if (d->mode == RADIX_SEARCH_MODE) {
        d->radix_root = createEmptyRadixNode();
        char *row;
        while((row = get_csv_row(csv)) != NULL) {
            suburb_t *data = parseRowToSuburb(row);
            char *key = get_key(data); 
            insert(d->radix_root, key, data);
            free(row);
        }
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
    assert(dict);
    assert(dict->radix_root || dict->list);
    // Iterate over each key to search 
    list_t *search_result;
    for(int i = 0; i < n_keys; i++) {
        comparison_count_t *count = NULL;
        if (COMPARE) {
            count = create_comparison_count();
        }

        if (dict->mode != RADIX_SEARCH_MODE) {
            search_result = search_all(dict->list, keys[i], count, cmpSuburbName, cpySuburb);
        } else if (dict->mode == RADIX_SEARCH_MODE) {
            search_result = search_radix(dict->radix_root, keys[i], count);
            // copy the search result to not free data refrence
            search_result = deep_copy_list(search_result, cpySuburb);
        } else {
            // not valid
            printf(" ERROR in searching dict\n");
            return;
        }
        output_search_result(keys[i], search_result, count, print_with_headers, f);
        free_list(search_result, free_suburb);
        
        if (COMPARE) {
            free_comparison_count(count);
        }
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



