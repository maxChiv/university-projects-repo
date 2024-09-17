/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    linkedlist.h :
                = the interface module for the linked list struct and operations
--------------------------------------------------------------------------- */

// Guard
#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_ 

#include <stdio.h>
#include <stdlib.h>
#include "comparison.h"


// Keep modular and indpendent of node and list tag.
typedef struct node node_t;
typedef struct list list_t;
typedef void *data_ptr; 

typedef int (*CompareFunc)(void *, void *, int *bit_count);
typedef void (*FreeFunc)(void *);
typedef void* (*CopyFunc)(void *);
typedef void (*PrintFunc)(FILE *f, void *);

// List Functions
int get_list_size(list_t *list);
list_t *make_empty_list(void);
int is_empty_list(list_t *list);
void free_list(list_t *list, FreeFunc free_data);
void print_list_data(FILE *f, list_t *list, PrintFunc print_data);
list_t *deep_copy_list(list_t *l, CopyFunc cpy);

// Insertion Functions
void prepend(list_t *list, data_ptr value);  
void append(list_t *list, data_ptr value);  
    // Can be changed to return a different value.

// Get Functions
data_ptr get_head(list_t *list);
data_ptr get_foot(list_t *list);
data_ptr search(list_t *list, void *key, CompareFunc cmpKey);
data_ptr search_node(node_t *node, void *key, CompareFunc cmpKey, 
    comparison_count_t *count);
list_t* search_all(list_t *list, void *key, comparison_count_t *count,
    CompareFunc cmpKey, CopyFunc cpy);
// Deletion Function
int rmv_all(list_t *list, void *key, CompareFunc cmpKey, FreeFunc free_data);

// For string keys
int min(int a, int b, int c);
int editDistance(char *str1, char *str2, int n, int m);
char *findClosestKey(list_t *list, char *original_key);

#endif

