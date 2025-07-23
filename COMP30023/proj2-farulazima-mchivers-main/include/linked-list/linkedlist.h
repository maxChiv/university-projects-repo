/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 2
    linkedlist.h :
                = the interface module for the linked list struct and operations

Note:
- Implementation is taken from an earlier personal project and optimised
  for this project.
--------------------------------------------------------------------------- */

// Guard
#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_ 

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SUCCESS 0
#define FAILURE -1

/* --- FORWARD DECLARATIONS --- */
typedef struct node node_t;
typedef struct list list_t;

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
// List Functions
list_t *list_create(void);
int is_empty_list(list_t *list);
void free_list(list_t *list);

// Insertion Functions
node_t* prepend(list_t *list, uint32_t value);  
node_t* append(list_t *list, uint32_t value);  

// Get Functions
int list_size(list_t *list);
uint32_t list_get_head(list_t *list);
uint32_t list_get_tail(list_t *list);
uint32_t fifo_pop_head(list_t *list);

#endif

