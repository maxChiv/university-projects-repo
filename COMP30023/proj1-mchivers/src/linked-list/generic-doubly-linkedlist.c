/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
	generic-doubly-linkedlist.c :
		= the implementation of module for doubly linked list and other operations.

Note:
- Implementation is taken from an earlier personal project and optimised
  for this project.
--------------------------------------------------------------------------- */
#include "linked-list/generic-doubly-linkedlist.h"

/* --- DEFINING STRUCTS --- */
struct node {
	void *data_ptr;  
	dll_node_t *prev;
	dll_node_t *next;
};

struct list {
	dll_node_t *head;  // DEFAULT AT NULL FOR EMPTY
	dll_node_t *tail;
};


/* --- FUNCTION IMPLEMENTATIONS --- */

/* Function to return the size of the list */
int dll_list_size(dll_list_t *list) {
	int size = 0;
	if (list == NULL) {
		return size;
	}
	dll_node_t *curr = list->head;

	while (curr != NULL) {
		size++;
		curr = curr->next;
	}

	return size;
}

/* Mallocs an empty list and returns a pointer to that list. */
dll_list_t *dll_list_create(void) {
	dll_list_t *list;
	list = (dll_list_t*)malloc(sizeof(*list));
	assert(list);
	list->head = list->tail = NULL;
	return list;
}

/* Checks if the list is empty and 
 *  Returns: 0 for not empty, 1 for empty 
 */
int dll_is_empty_list(dll_list_t *list) {
	assert(list);
	return list->head == NULL;
}

/* Deallocs the memory associated with a node
 */
void dll_free_node(dll_node_t *node, FreeFunc free_data) {
    free_data(node->data_ptr);
    free(node);
}

/* Deallocs the memory associated with this list.
 */
void dll_free_list(dll_list_t *list, FreeFunc free_data) {
	assert(list);
	dll_node_t *curr, *next;
	curr = list->head;

	while(curr) {
		// save next node pointer before freeing current
		next = curr->next;
		// free current node
        dll_free_node(curr, free_data);
		// move to next node
		curr = next;
	}
	free(list);
}

/* Inserts the value at the head of the list, and moves nodes accordingly. 
Returns the node created for the data. */
dll_node_t* dll_prepend(dll_list_t *list, void *data_ptr) {
	dll_node_t *new;
	new = (dll_node_t*)malloc(sizeof(*new));
	assert(list && new);
	
	new->data_ptr = data_ptr;
	new->prev = NULL;
	new->next = list->head;
	
	if (list->head != NULL) {
		// Update previous pointer of old head
		list->head->prev = new;
	}
	
	list->head = new;

	if (list->tail == NULL) {
		// This is the first insertion into the list
		list->tail = new;
	}
	
	return new;
}

/* Inserts the value at the tail of the list. Returns the node created for the 
data. */
dll_node_t* dll_append(dll_list_t *list, void *data_ptr) {
  dll_node_t *new;
	new = (dll_node_t*)malloc(sizeof(*new));
	assert(list && new);
	
	new->data_ptr = data_ptr;
	new->next = NULL;
	new->prev = list->tail;

	if (list->head == NULL) {
		// This is the first insertion into the list
		list->head = list->tail = new;
	} else {
		list->tail->next = new;
		list->tail = new;
	}
	
	return new;
}

/* Retrieves the head node data value, and doesn't remove the node */
void* dll_list_get_head(dll_list_t *list) {
	assert(list && list->head);
	return list->head->data_ptr;
}

/* Retrieves the tail node data value, and doesn't remove the node */
void* dll_list_get_tail(dll_list_t *list) {
	assert(list && list->tail);
	return list->tail->data_ptr;
}

/* Retrieves node data value, and doesn't remove the node */
void* dll_get_data_ptr(dll_node_t *node) {
    return node->data_ptr;
}

/* Retrieves the head, and removes it from the list. Similar to a FIFO approach. */
void* dll_fifo_pop_head(dll_list_t *list) {
	assert(list != NULL);
	assert(list->head != NULL); // Can't pop from an empty list

	dll_node_t *old_head = list->head;
	void *data_ptr = old_head->data_ptr;

	// Move head to next node
	list->head = old_head->next;

	// If the list becomes empty, update the tail too
	if (list->head == NULL) {
		list->tail = NULL;
	} else {
		// Update prev pointer of new head to NULL
		list->head->prev = NULL;
	}

	// Free the node and return its data
	free(old_head);
	return data_ptr;
}

/* Remove a specific node from the list */
void* dll_remove_node(dll_list_t *list, dll_node_t *node) {
	assert(list && node);
	
	void *data_ptr = node->data_ptr;
	
	// If node is head, update head
	if (node == list->head) {
		list->head = node->next;
	}
	
	// If node is tail, update tail
	if (node == list->tail) {
		list->tail = node->prev;
	}
	
	// Update surrounding nodes' pointers
	if (node->prev != NULL) {
		node->prev->next = node->next;
	}
	
	if (node->next != NULL) {
		node->next->prev = node->prev;
	}
	
	free(node);
	return data_ptr;
}