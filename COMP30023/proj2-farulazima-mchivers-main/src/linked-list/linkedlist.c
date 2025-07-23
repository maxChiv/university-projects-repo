/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 2
    linkedlist.c :
            = the implementation of module sinlgy linked list and other operations.

Note:
- Implementation is taken from an earlier personal project and optimised
  for this project.
--------------------------------------------------------------------------- */

#include "linked-list/linkedlist.h"

/* --- DEFINING STRUCTS --- */
struct node {
    uint32_t data;
    node_t *next;
};

struct list {
    node_t *head;  // DEFAULT AT NULL FOR EMPTY
    node_t *tail;
};


/* --- FUNCTION IMPLEMENTATIONS --- */

/* Function to return the size of the list */
int list_size(list_t *list) {
    int size = 0;
    if (list == NULL) {
        return size;
    }
    node_t *curr = list->head;

    while (curr != NULL) {
        size++;
        curr = curr->next;
    }

    return size;
}

/* Mallocs an empty list and returns a pointer to that list. */
list_t *list_create(void) {
    list_t *list;
    list = (list_t*)malloc(sizeof(*list));
    assert(list);
    list->head = list->tail = NULL;
    return list;
}

/* Checks if the list is empty and 
    returns: 0 for not empty, 1 for empty */
int is_empty_list(list_t *list) {
    assert(list);
    return list->head == NULL;
}

/* Deallocs the memory associated with this list. */
void free_list(list_t *list) {
    assert(list);
    node_t *curr, *prev;
    curr = list->head;

    while(curr) {
        // a node to delete 
        prev = curr;
        // check the next node
        curr = curr -> next;
        free(prev);
    }
    free(list);
}

/* Inserts the value at the head of the list, and moves nodes accordingly. 
Returns the node created for the data. */
node_t* prepend(list_t *list, uint32_t data) {
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
    assert(list && new);
    new->data = data;
    new->next = list->head;
    list->head = new;

    if (list->tail == NULL) {
        // This is the first insertion into the list
        list->tail = new;
    }
    return new;
}

/* Inserts the value at the tail of the list. Returns the node created for the 
data. */
node_t* append(list_t *list, uint32_t data) {
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
    assert(list && new);
    new->data = data;
    new->next = NULL;

    if (list->head == NULL) {
        // This is the first insertion into the list
        list->head = list->tail = new;
    } else {
        list->tail->next = new;
        list->tail = new;
    }
    return new;
}

/* Retrieves the head node data value, and doesnt remove the node */
uint32_t list_get_head(list_t *list) {
    assert(list && list->head);
    return list->head->data;
}

/* Retrieves the tail node data value, and doesnt remove the node */
uint32_t list_get_tail(list_t *list) {
    assert(list && list->tail);
    return list->tail->data;
}

/* Retrieves the tail, and removes it from the list. Similar to a FIFO approach. */
uint32_t fifo_pop_head(list_t *list) {
    assert(list != NULL);
    assert(list->head != NULL); // Can't pop from an empty list

    node_t *old_head = list->head;
    uint32_t data = old_head->data;

    // Move head to next node
    list->head = old_head->next;

    // If the list becomes empty, update the tail too
    if (list->head == NULL) {
        list->tail = NULL;
    }

    // Free the node and return its data
    free(old_head);
    return data;
}