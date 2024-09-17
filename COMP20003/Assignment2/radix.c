/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 2
    radix.c :
            = the implementation of radix module for Assignment 2
--------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "radix.h"

struct RadixNode {
    char *prefix;  // Prefix stored as raw bytes
    unsigned int prefixLength;    // Length of the prefix in bits
    list_t *list;             // Pointer to a list with data, NULL if not there
    radix_node_t *left; // Pointer to the left child node (0) 
    radix_node_t *right;// Pointer to the right child node (1)
};

/* Creates an empty radix node */
radix_node_t* createEmptyRadixNode() {
    // Allocate memory for the new node
    radix_node_t *node = (radix_node_t*)malloc(sizeof(radix_node_t));
    assert(node); // Ensure memory allocation was successful

    // Initialize the fields of the node
    node->prefix = NULL;       // No prefix yet
    node->prefixLength = 0;    // No prefix length
    node->list = NULL;         // No associated data
    node->left = NULL;         // No left child
    node->right = NULL;        // No right child

    return node;
}

/* Creates and fills a radix node with relevant data */
radix_node_t* createRadixNode(char *key, unsigned int key_pos, void *data) {
        // Empty radix trie 
        radix_node_t *node = createEmptyRadixNode();

        if (node->list == NULL) {
            node->list = make_empty_list();
        }
        prepend(node->list, data);

        node->prefixLength =  getNumberOfBits(key, key_pos);
        node->prefix = createStem(key, key_pos, node->prefixLength);
        return node;
}

/* Frees the radix node */
void freeRadixNode(radix_node_t *node, FreeFunc free_data) {
    if (node == NULL) {
        return;
    }
    // Free the prefix string
    if (node->prefix != NULL) {
        free(node->prefix);
    }
    // Free the list
    if (node->list != NULL) {
        free_list(node->list, free_data);
    }
    // Recursively free the left and right child nodes
    if (node->left != NULL) {
        freeRadixNode(node->left, free_data);
    }
    if (node->right != NULL) {
        freeRadixNode(node->right, free_data);
    }
    free(node);
}

/* Compares a nodes prefix with a string key and comapres each bit
    Returns: - the bit index of a mismatch from the key_bit_pos
             - the entire length of comparison if reaching the end
             - -1 if they are identical
*/
int comparePrefix(radix_node_t *node, char *key, unsigned int key_bit_pos) {
    assert(node && key);

    unsigned int prefix_length = node->prefixLength;
    unsigned int key_length = getNumberOfBits(key, key_bit_pos);
    unsigned int max_length;

    // Find the minimum bits, which is the maximum we can compare 
    if (prefix_length > key_length) {
        max_length = key_length;
    } else {
        max_length = prefix_length;
    }

    // Comapre each bit in the prefix with the key
    unsigned int bitIndex;
    for (bitIndex = 0; bitIndex < max_length; bitIndex++) {
        int nodeBit = getBit(node->prefix, bitIndex);
        int keyBit = getBit(key, bitIndex + key_bit_pos);

        if (nodeBit != keyBit) {
            // Mismatch so return the current index
            return bitIndex;
        }
    }
    // Reaching here ... the prefixes are identical up to max_length

    if ((prefix_length == max_length && key_length == max_length)) {
        // The node's prefix and the key are exactly the same
        return IDENTICAL_BIT_CMP;
    } 

    // The prefix is within the key or we have reached the end of the key 
    return max_length;
}


/* Determines if the node is a final node (leaf node)
    - Returns 0 if it is not a leaf node 
    - Returns 1 if it is a leaf node 
*/
int isLeaf(radix_node_t *node) {
        if (node == NULL) {
        return 0;
    }

    // Check if the node has no children
    if (node->left != NULL || node->right != NULL) {
        return 0;
    }
    return 1;
}

/* Advances to the next node based on the bit value in the key at the given index.
   - Returns the left child node if the bit is 0
   - Returns the right child node if the bit is 1
*/
radix_node_t* advanceDown(radix_node_t *node, char *key, unsigned int index) {
    if (node == NULL) {
        return NULL;
    }

    // Get the bit value from the key at the given index
    assert(index <= getNumberOfBits(key, 0));  // Check no out of bounds
    int bit = getBit(key, index);

    // Traverse to the left or right child based on the bit value
    if (bit == 0) {
        return node->left;  // Advance down the left branch
    } else {
        return node->right; // Advance down the right branch
    }
}

/* Traverses the radix trie to find the node where the insertion of the key
should be. It also assigns key_index the index within the key where the
mismatch for the node occured */
radix_node_t *findInsertionNode(radix_node_t *root, char* key, 
    unsigned int *key_index, unsigned int *insert_node_index) {
    
    assert(root && key);
    radix_node_t *curr = root;
    unsigned int curr_key_pos = 0; // Position in the key

    while (curr != NULL) {
        int bitIndexResult = comparePrefix(curr, key, curr_key_pos);
        if (bitIndexResult == IDENTICAL_BIT_CMP && isLeaf(curr)) {
            // Key is identical and found an existing node to key
            *key_index = *insert_node_index = IDENTICAL_BIT_CMP;
            return curr;
        }

        if (bitIndexResult < curr->prefixLength) {
            // The key is within this current node and need to split the node
            *key_index = curr_key_pos + bitIndexResult;
            *insert_node_index = bitIndexResult;
            return curr;
        } else {
            // The key is beyond the current node's prefix, so check next node
            curr_key_pos += bitIndexResult;
            radix_node_t *next = advanceDown(curr, key, curr_key_pos);
            if (next == NULL) {
                // The current node does not have a child for this key
                return curr;
            } 
            curr = next;
        }
    }
    
    return NULL;
}

/* Inserts the data at the appropriate place, based on its string key */
void insert(radix_node_t *root, char *key, void* data) {
    assert(root != NULL);
    assert(key != NULL);

    if (root->prefix == NULL) {
        // No data in radix trie
        root->prefixLength =  getNumberOfBits(key, 0);
        root->prefix = createStem(key, 0, root->prefixLength);
        if(root->list == NULL) {
            root->list = make_empty_list();
        }   
        prepend(root->list, data);
        return;
    }
    
    unsigned int key_index;
    unsigned int insert_node_index;
    radix_node_t *insert_node = findInsertionNode(root, key, &key_index, 
        &insert_node_index);

    if (key_index == IDENTICAL_BIT_CMP) {
        // Key already exists, so insert the data into the list of the node
        assert(insert_node->list);  // Should already be here 
        prepend(insert_node->list, data);
        return;
    }
    
    // Determine if a split is needed 
    radix_node_t *next_child = advanceDown(insert_node, key, key_index);
    if (next_child == NULL && insert_node->prefixLength == insert_node_index) {
        // The insertNode does not have a child for this key 
        // Check which child 
        if (getBit(key, key_index)) {
            // go to right child
            insert_node->right = createRadixNode(key, key_index, data);
        } else {
            // go to left child
            insert_node->left = createRadixNode(key, key_index, data);
        }
    } else {
        // There is an existing child node, so perform a split if necessary
        splitNode(insert_node, key, key_index, insert_node_index, data);
    }
}

void splitNode(radix_node_t *target_node, char *key, unsigned int key_index, 
    unsigned int target_node_index, void *data) {
    assert(target_node != NULL);
    assert(key != NULL);
    assert(target_node_index < target_node->prefixLength); 
    
    // Create the altered node from the target node by transfering appropriate attributes
    radix_node_t *alt_node = createEmptyRadixNode();
    alt_node->prefixLength = target_node-> prefixLength - target_node_index;
    alt_node->prefix = createStem(target_node->prefix, target_node_index, 
        alt_node->prefixLength);
    alt_node->list = target_node->list;
    alt_node->left = target_node->left;
    alt_node->right = target_node->right;
    
    // Create new node for new data 
    radix_node_t *new_insert_node = createRadixNode(key, key_index, data);

    // Reset target_node 
    target_node->left = target_node->right = NULL;
    target_node->list = NULL;
    target_node->prefixLength = target_node_index;
    char *tmp_prefix = target_node->prefix;
    target_node->prefix = createStem(target_node->prefix, 0, 
        target_node->prefixLength);
    free(tmp_prefix);

    // Place the split nodes in the appropriate place in the target_node 
    if (getBit(new_insert_node->prefix, 0)) {
        // new node is right branch
        target_node->right = new_insert_node;
        target_node->left = alt_node;
    } else {
        // new node is left branch
        target_node->left = new_insert_node;
        target_node->right = alt_node;
    }
}

/* Searching Functions */

/* Function to recursively traverse the trie and collect words starting from a 
   specific node and all its descendants, stored in a list passed through */
void collectWords(radix_node_t *node, char *curr_prefix, unsigned int curr_len,
                  radix_node_t *start_node, list_t *word_list) {
    if (!node) return; // Base case: return if the node is NULL
    assert(word_list);

    // Combine the current prefix with this node's prefix
    char *combined_prefix;
    unsigned int combined_len;

    if (curr_prefix == NULL) {
        // Starting from the root or initial node with no current prefix
        combined_prefix = createStem(node->prefix, 0, node->prefixLength);
        combined_len = node->prefixLength;
    } else {
        // Combine the existing prefix with the current node's prefix
        combined_prefix = combinePrefixes(curr_prefix, curr_len, 
                                           node->prefix, node->prefixLength, &combined_len);
    }

    // Check if we have reached the node after which we need to start collecting words
    if (node == start_node) {
        // Start collecting words from this node
        if(isLeaf(start_node)) {
            // Already at the end of the combined word
            append(word_list, combined_prefix);
            return;
        }

        collectWordsFromNode(node->left, combined_prefix, combined_len, word_list);
        collectWordsFromNode(node->right, combined_prefix, combined_len, word_list);
    } else {
        // Continue traversing to find the start node
        collectWords(node->left, combined_prefix, combined_len, start_node, word_list);
        collectWords(node->right, combined_prefix, combined_len, start_node, word_list);
    }

    // Free the combined prefix as it's dynamically allocated
    free(combined_prefix);
}

/* Function to recursively traverse the trie and collect words from descendents 
of node which is stored in a list passed through as a parameter */
void collectWordsFromNode(radix_node_t *node, char *curr_prefix, unsigned int curr_len, list_t *word_list) {
    if (!node) return;
    assert(word_list);

    // Combine the current prefix with this node's prefix
    char *combined_prefix;
    unsigned int combined_len;
    if (node != NULL && curr_prefix == NULL) {
        // At the root node
        combined_prefix = createStem(node->prefix, 0, node->prefixLength);
        combined_len = node ->prefixLength;
    } else {
        // Combine the existing prefix with the current node's prefix
        combined_prefix = combinePrefixes(curr_prefix, curr_len, 
                            node->prefix, node->prefixLength, &combined_len);
    }

    // If this node is at the end, add the combined prefix to the list of words
    if (isLeaf(node)) {
        append(word_list, combined_prefix);
        return;
    }

    // Recursively collect words from the left and right children
    collectWordsFromNode(node->left, combined_prefix, combined_len, word_list);
    collectWordsFromNode(node->right, combined_prefix, combined_len, word_list);

    // Free the combined prefix as it's dynamically allocated
    free(combined_prefix);
}


/* Searches through the redix trie via a key, and returns the list of data 
associated with that key, or the closest data with that key with a close edit 
distance. If no data, it returns null. */
list_t *search_radix(radix_node_t *root, char *key, comparison_count_t *count) {
    if (root == NULL) {
        return NULL;
    }

    radix_node_t *curr = root;
    unsigned int key_bit_idx = 0;  // Current position in the key
    increment_string_comparisons(count);
    while (curr) {

        // Compare the current node's prefix with the key
        int cmp_result = comparePrefix(curr, key, key_bit_idx);

        if (cmp_result != IDENTICAL_BIT_CMP) {
            key_bit_idx += cmp_result;
        }

        increment_node_accesses(count);

        if (cmp_result == IDENTICAL_BIT_CMP) {
            // Incrementing bit count 
            for (int i = 0; i < curr->prefixLength; i++) {
                increment_bit_comparisons_by_1(count);
                
            }
            // Identical prefix 
            if (isLeaf(curr)) {
                // Found the matched node
                return curr->list;
            }
            // Otherwise note found yet, continue down trie
            curr = advanceDown(curr, key, key_bit_idx);

        } else if (cmp_result == curr->prefixLength) {
            // Incrementing bit count 
            for (int i = 0; i < cmp_result; i++) {
                increment_bit_comparisons_by_1(count);
            }
            // Continue through the radix trie 
            radix_node_t *next = advanceDown(curr, key, key_bit_idx);
            if (next == NULL) {
                // Gone to a wrong node
                break;
            }
            curr = next;
        } else {
            // Incrementing bit count 
            for (int i = 0; i < cmp_result; i++) {
                increment_bit_comparisons_by_1(count);
            }
            if (getBit(curr->prefix, cmp_result) != getBit(key, key_bit_idx)) {
                // mismatch accord, break to go to closest key
                break;
            }
        }
    }
    // No exact match found
    assert(curr);
    // Retrieve all keys passing through node curr.
    list_t *keys = make_empty_list();
    collectWords(root, NULL, 0, curr, keys);
    char *next_key = strdup(findClosestKey(keys, key));
    free_list(keys, free_str);
    
    if (strcmp(key, next_key) == 0) {
        return NULL;  // avoid infinite loop.
    }

    // return the exact match for closest key
    reset_comparison_count(count);
    list_t *result = search_radix(root, next_key, count);
    free(next_key);  // Free the duplicated key after use
    return result;
}

void free_str(void *str) {
    free(str);
}

/* Printing Functions */
// Helper function to print a binary string representation of the prefix
void printBinaryPrefix(char *prefix, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        unsigned int byteIndex = i / 8;
        unsigned int bitIndex = 7 - (i % 8);
        int bit = (prefix[byteIndex] >> bitIndex) & 1;
        printf("%d", bit);
    }
}

// Recursive function to print the trie
void printTrieHelper(radix_node_t *node, int level) {
    if (node == NULL) {
        return;
    }

    // Print the prefix in binary with indentation based on level in the tree
    printf("%*sPrefix: ", level * 2, "");
    printBinaryPrefix(node->prefix, node->prefixLength);
    printf("\n");

    // Recur for left and right children
    printTrieHelper(node->left, level + 1);
    printTrieHelper(node->right, level + 1);
}

void printTrie(radix_node_t *root) {
    printTrieHelper(root, 0);
}



