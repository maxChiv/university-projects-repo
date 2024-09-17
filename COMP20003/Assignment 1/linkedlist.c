/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 1
    linkedlist.c :
            = the implementation of module sinlgy linked list and other operations.
--------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linkedlist.h"

// define node struct tag
struct node {
    data_ptr data;
    node_t *next;
};

// define list struct tag, 
struct list {
    node_t *head;  // DEFAULT AT NULL FOR EMPTY
    node_t *foot;
};


/* Function Implementations ----*/


/* Function to return the size of the list */
int get_list_size(list_t *list) {
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
list_t *make_empty_list(void) {
    list_t *list;
    list = (list_t*)malloc(sizeof(*list));
    assert(list);
    list->head = list->foot = NULL;
    return list;
}

/* Checks if the list is empty and 
    returns: 0 for not empty, 1 for empty */
int is_empty_list(list_t *list) {
    assert(list);
    return list->head == NULL;
}

/* Deallocs the memory associated with this list. */
void free_list(list_t *list, FreeFunc free_data) {
    assert(list);
    node_t *curr, *prev;
    curr = list->head;

    while(curr) {
        // a node to delete 
        prev = curr;
        // check the next node
        curr = curr -> next;
        free_data(prev->data); 
        free(prev);
    }
    free(list);
}

/* Prints all the data with the provided function by traversing list */
void print_list_data(FILE *f, list_t *list, PrintFunc print_data) {
    if (list == NULL) { return; }
    node_t *curr = list->head;
    while(curr) {
        print_data(f, curr->data);
        curr = curr->next;
    }
}

/* Inserts the value at the head of the list, and moves nodes accordingly. */
void prepend(list_t *list, data_ptr data) {
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
    assert(list && new);
    new->data = data;
    new->next = list->head;
    list->head = new;

    if (list->foot == NULL) {
        // This is the first insertion into the list
        list->foot = new;
    }
    return;
}

/* Inserts the value at the foot of the list. */
void append(list_t *list, data_ptr data) {
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
    assert(list && new);
    new->data = data;
    new->next = NULL;

    if (list->head == NULL) {
        // This is the first insertion into the list
        list->head = list->foot = new;
    } else {
        list->foot->next = new;
        list->foot = new;
    }
    return;
}

/* Retrieves the head node data value, and doesnt remove the node */
data_ptr get_head(list_t *list) {
    assert(list && list->head);
    return list->head->data;
}

/* Retrieves the foot node data value, and doesnt remove the node */
data_ptr get_foot(list_t *list) {
    assert(list && list->foot);
    return list->foot->data;
}

/* Retrieves the first occurence of a data that matches the search key
    - Return NULL if not found, or the first pointer that matches
    - Note: void* usage can bring risks! */
data_ptr search(list_t *list, void *key, CompareFunc cmpKey) {
    assert(list && list->head);
    /* list has at least one node. */
    node_t *curr = list->head;
    while(curr && cmpKey(key, curr->data) != 0) {
        /* Have not found a match */
        curr = curr->next;
    }
    if (curr) {
        return curr->data; 
    } else {
        return NULL;
    }
}

/* Retrieves the data if it matches the search key, at a particular node
    - Return NULL if not found, or the first pointer that matches
    - Note: void* usage can bring risks! */
data_ptr search_node(node_t *node, void *key, CompareFunc cmpKey) {
    if (!node) {
        return NULL;  
    }
    if (cmpKey(key, node->data) == 0) {
        return node->data; 
    } else {
        return NULL;
    }
}

/* Retrieves all occurences of matching data from the search key in a list 
    - Return  an empty list if not found, 
    or a pointer to a NEW list of data that match, in preserved order.
    - Note: void* usage can bring risks! */
list_t* search_all(list_t *list, void *key, CompareFunc cmpKey, CopyFunc cpy) {
    assert(list);
    list_t *result_lst = NULL;

    result_lst = make_empty_list();

    node_t *curr = list->head;
    data_ptr curr_result = NULL; 
    while(curr) {
        if (curr && (curr_result = search_node(curr, key, cmpKey)) != NULL) {
            // a data we can add to result
            append(result_lst, cpy(curr_result));
        }
        curr = curr->next;
    }
    return result_lst;
}

/* Removes all instances of data matching to the key, and delinks their nodes to
be placed into the returning list. It frees all delinked nodes.
    - Returns an integer of the number of nodes deleted */
int rmv_all(list_t *list, void *key, CompareFunc cmpKey, FreeFunc free_data) {
    assert(list);
    int n_removed = 0;
    if(is_empty_list(list)) {
        return n_removed;
    }
    
    // At least one node in list
    node_t *curr = list->head;
    data_ptr curr_result = NULL; 
    node_t *prev = NULL;

    while(curr) {
        if (curr && (curr_result = search_node(curr, key, cmpKey)) != NULL) {
            node_t *tmp = curr;

            if (prev == NULL) {  // Head node is the match
                list->head = curr->next;
                if (list->head == NULL) {  // Head was foot
                    list->foot = NULL;
                }
            } else {
                // Delink the matching node
                prev->next = curr->next;
                if (curr->next == NULL) {  // foot node removed
                    list->foot = prev;
                }
            }

            n_removed++;
            free_data(curr_result);
            free(tmp);

            // Do not update curr, as it has already been removed
            curr = (prev == NULL) ? list->head : prev->next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }

    return n_removed;
}
