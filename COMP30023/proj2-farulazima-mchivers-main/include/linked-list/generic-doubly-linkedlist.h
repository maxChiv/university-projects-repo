/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 2
    generic-doubly-linkedlist.h :
                = the interface module for the doubly linked list struct and operations.

Note:
- Implementation is taken from an earlier personal project and optimised
  for this project.
--------------------------------------------------------------------------- */
#ifndef _GENERIC_DOUBLY_LINKEDLIST_H_
#define _GENERIC_DOUBLY_LINKEDLIST_H_

#include <stdlib.h>
#include <assert.h>

/* --- FORWARD DECLARATIONS --- */
typedef struct node dll_node_t;
typedef struct list dll_list_t;

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
typedef void (*FreeFunc)(void *);  // Needed for free function.

// Management Functions
int dll_list_size(dll_list_t *list);
dll_list_t *dll_list_create(void);
int dll_is_empty_list(dll_list_t *list);
void dll_free_node(dll_node_t *node, FreeFunc free_data);
void dll_free_list(dll_list_t *list, FreeFunc free_data);

// Insertion functions.
dll_node_t* dll_prepend(dll_list_t *list, void *data_ptr);
dll_node_t* dll_append(dll_list_t *list, void *data_ptr);

// Get functions.
void* dll_list_get_head(dll_list_t *list);
void* dll_list_get_tail(dll_list_t *list);
void* dll_get_data_ptr(dll_node_t *node);

// Remove functions.
void* dll_remove_node(dll_list_t *list, dll_node_t *node);
void* dll_fifo_pop_head(dll_list_t *list);

#endif 