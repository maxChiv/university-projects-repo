/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    radix.h :
            = the interface module for the radix trie of Assignment 2
--------------------------------------------------------------------------- */

// Guard
#ifndef _RADIX_H_
#define _RADIX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "linkedlist.h"
#include "comparison.h"
#include "bitoperations.h"

typedef struct RadixNode radix_node_t;
typedef void (*FreeFunc)(void *);

// Function prototypes

radix_node_t* createEmptyRadixNode();
radix_node_t* createRadixNode(char *key, unsigned int key_pos, void *data);
void freeRadixNode(radix_node_t *node, FreeFunc free_data);
int comparePrefix(radix_node_t *node, char *key, unsigned int key_bit_pos);
int isLeaf(radix_node_t *node);
radix_node_t* advanceDown(radix_node_t *node, char *key, unsigned int index);
radix_node_t *findInsertionNode(radix_node_t *root, char* key, unsigned int *key_index, unsigned int *insert_node_index);
void insert(radix_node_t *root, char *key, void* data);
void splitNode(radix_node_t *target_node, char *key, unsigned int key_index, unsigned int target_node_index, void *data);
void collectWords(radix_node_t *node, char *curr_prefix, unsigned int curr_len, radix_node_t *start_node, list_t *word_list);
void collectWordsFromNode(radix_node_t *node, char *curr_prefix, unsigned int curr_len, list_t *word_list);

list_t *search_radix(radix_node_t *root, char *key, comparison_count_t *count);
void free_str(void *str);

void printTrieHelper(radix_node_t *node, int level);
void printTrie(radix_node_t *root);

#endif
