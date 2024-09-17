/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
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
    assert(list);
    assert(new);
    new->data = data;
    new->next = list->head;
    list->head = new;

    if (list->foot == NULL) {
        // This is the first insertion into the list
        list->foot = new;
    }
    return;
}

/* Copies all data into a deep copy of the list without freeing original data.
   Returns a pointer to the new deep copy list. */
list_t *deep_copy_list(list_t *l, CopyFunc cpy) {
    if (l == NULL || l->head == NULL) {
        return make_empty_list();  // Return an empty list if the original list is empty
    }

    // Create a new list for the deep copy
    list_t *new_list = make_empty_list();

    // Iterate through the original list
    node_t *curr = l->head;
    while (curr != NULL) {
        // Deep copy the data in the current node using the CopyFunc
        void *copied_data = cpy(curr->data);

        // Append the copied data to the new list
        append(new_list, copied_data);

        // Move to the next node in the original list
        curr = curr->next;
    }

    return new_list;  // Return the new deep copy list
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
    while(curr && cmpKey(key, curr->data, NULL) != 0) {
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
data_ptr search_node(node_t *node, void *key, CompareFunc cmpKey, 
    comparison_count_t *count) {
    if (!node) {
        return NULL;  
    }

    if (count) {
        increment_node_accesses(count);
        // BAD CODE: assuming key is string
        increment_string_comparisons(count);
    }

    if (count) {
        if (cmpKey(key, node->data, get_bit_comparisons(count)) == 0) {
            return node->data;
        }
    } else {
        if (cmpKey(key, node->data, NULL)) {
            return node->data;
        }
    }
    return NULL;
}

/* Retrieves all occurences of matching data from the search key in a list 
    - Return  an empty list if not found, 
    or a pointer to a NEW list of data that match, in preserved order.
    - Note: void* usage can bring risks! */
list_t* search_all(list_t *list, void *key, comparison_count_t *count,
    CompareFunc cmpKey, CopyFunc cpy) {
    assert(list);
    list_t *result_lst = NULL;

    result_lst = make_empty_list();

    node_t *curr = list->head;
    data_ptr curr_result = NULL; 
    while(curr) {
        if (curr && (curr_result = search_node(curr, key, cmpKey, count)) != NULL) {
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
        if (curr && (curr_result = search_node(curr, key, cmpKey, NULL)) != NULL) {
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


/* Returns min of 3 integers 
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int min(int a, int b, int c) {
    if (a < b) {
        if(a < c) {
            return a;
        } else {
            return c;
        }
    } else {
        if(b < c) {
            return b;
        } else {
            return c;
        }
    }
}

/* Returns the edit distance of two strings
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int editDistance(char *str1, char *str2, int n, int m){
    assert(m >= 0 && n >= 0 && (str1 || m == 0) && (str2 || n == 0));
    // Declare a 2D array to store the dynamic programming
    // table
    int dp[n + 1][m + 1];

    // Initialize the dp table
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            // If the first string is empty, the only option
            // is to insert all characters of the second
            // string
            if (i == 0) {
                dp[i][j] = j;
            }
            // If the second string is empty, the only
            // option is to remove all characters of the
            // first string
            else if (j == 0) {
                dp[i][j] = i;
            }
            // If the last characters are the same, no
            // modification is necessary to the string.
            else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = min(1 + dp[i - 1][j], 1 + dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
            // If the last characters are different,
            // consider all three operations and find the
            // minimum
            else {
                dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
        }
    }

    // Return the result from the dynamic programming table
    return dp[n][m];
}

/* Function to find the closest string key in the list based on edit distance. 
Assumes that the list has data of char* */
char *findClosestKey(list_t *list, char *original_key) {
    if (list == NULL) {
        return NULL;
    }

    node_t *current = list->head;
    char *closest_key = NULL;
    int min_distance = 0;

    while (current != NULL) {
        int distance = editDistance(original_key, ((char*)current->data), 
                        strlen(original_key), strlen(((char*)current->data)));

        if (closest_key == NULL || distance < min_distance) {
            min_distance = distance;
            closest_key = current->data;
        } else if (distance == min_distance) {
            // Equal, so change to the alphabetically earliest result
            if (strcmp(current->data, closest_key) < 0) {
                closest_key = current->data;
            }
        }

        current = current->next;
    }

    return (char*)closest_key;
}
