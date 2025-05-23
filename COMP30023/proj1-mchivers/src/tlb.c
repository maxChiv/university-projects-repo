/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP30023 Assignment 1
    tlb.c :
            = the implementation for translation lookaside buffer (tlb).

Note:
    - This implementation attempts to replicate the tlb using a doubly-linked
      list for the LRU replacement policy, and a hashing function, to achieve 
      the below operation time complexities:
    
        Operation         Data Structure Used      Time Complexity
        ----------------------------------------------------------
        Lookup page          Hash                     O(1)
        Insert new page      List + Hash map          O(1)
        Evict page           Tail of list             O(1)
        Update usage (LRU)   Move node to head        O(1)
--------------------------------------------------------------------------- */
#include "tlb.h"

/* --- INTERNAL FUNCTION PROTOTYPES --- */
tlb_entry_t* get_entry_from_node(dll_node_t *node);
void tlb_free_entry(void* entry);
int tlb_hash(uint32_t page_number);
void tlb_free_hash_entry(tlb_hash_entry_t *hash_entry);
tlb_entry_t* tlb_create_entry(uint32_t page_number, uint32_t frame_number);


/* --- DEFINING STRUCTS --- */
struct tlb_hash_entry {
    dll_node_t *node_to_data;               // Points to the dll_node in the LRU which stores data.
    tlb_hash_entry_t *next_in_chain;        // For collision chaining
};

struct tlb_entry {
    uint32_t page_number;
    uint32_t frame_number;
};

struct tlb {
    dll_list_t *LRU;
    tlb_hash_entry_t* hashmap[TLB_NUMBER_OF_ENTRIES];  // Hashmap to store pointers to LRU nodes
    uint32_t tlb_size;
};

/* --- FUNCTION IMPLEMENTATIONS --- */

/** tlb_hash - computes the hash index for a given page number for the tlb
 */
int tlb_hash(uint32_t page_number) {
    // A random hash using bit mixing and XOR'ing to try distribute more accurately.
    return ((page_number >> 3) ^ page_number) % TLB_NUMBER_OF_ENTRIES;
}

/** create_tlb - allocates and initialises a new TLB structure
 *  Returns:
 *      Pointer to the newly created TLB, or NULL on allocation failure
 */
tlb_t* tlb_create(void) {
    tlb_t *tlb = malloc(sizeof(tlb_t));
    if (tlb == NULL) return NULL;

    tlb->LRU = dll_list_create(); 
    if (tlb->LRU == NULL) {
        free(tlb);
        return NULL;
    }

    for (int i = 0; i < TLB_NUMBER_OF_ENTRIES; i++) {
        tlb->hashmap[i] = NULL;
    }

    tlb->tlb_size = 0;

    return tlb;
}

/** tlb_free_hash_entry - frees a hash entry and its associated data.
 */
void tlb_free_hash_entry(tlb_hash_entry_t *hash_entry) {
    if (hash_entry == NULL) return;

    // Free the LRU node and the TLB entry inside it
    if (hash_entry->node_to_data != NULL) {
        dll_free_node(hash_entry->node_to_data, tlb_free_entry);
    }
    // Free the hash entry itself
    free(hash_entry);
}

/** tlb_free_entry - frees the allocated memory of the entry.
 *  - specifically used within the dll_list_t type.
 */
void tlb_free_entry(void* entry) {
    free((tlb_entry_t*)entry);
}

/** tlb_destroy - frees all memory used by the TLB, including the LRU list
 *  and the hash map with any chained entries.
 */
 void tlb_destroy(tlb_t *tlb) {
    if (tlb == NULL) return;

    // Free all entries in the LRU list (this will call tlb_free_entry on each node's data)
    dll_free_list(tlb->LRU, tlb_free_entry);

    // Free any chained entries in the hashmap
    for (int i = 0; i < TLB_NUMBER_OF_ENTRIES; i++) {
        tlb_hash_entry_t *entry = tlb->hashmap[i];
        while (entry != NULL) {
            tlb_hash_entry_t *next = entry->next_in_chain;
            free(entry);
            entry = next;
        }
    }

    free(tlb);
}

/** get_entry_from_node - extracts the TLB entry stored in the given DLL node.
 *  Parameters:
 *      node - a pointer to the dll_node_t containing a TLB entry
 *  Returns:
 *      A pointer to the tlb_entry_t stored in the node.
 */
 tlb_entry_t* get_entry_from_node(dll_node_t *node) {
    if (node == NULL) return NULL;
    return (tlb_entry_t*)dll_get_data_ptr(node);
}

/** tlb_get_LRU_page - Retrieves the least recently used page number from the TLB.
 */
uint32_t tlb_get_LRU_page(tlb_t *tlb) {
    return ((tlb_entry_t*)dll_list_get_head(tlb->LRU))->page_number;
}

/** tlb_get_size - Retrieves the size of the TLB.
 */
uint32_t tlb_get_size(tlb_t *tlb) { return tlb->tlb_size; }


/** tlb_lookup - searches for a page number in the TLB and returns its frame number if found.
 * - Updates based of LRU when TLB-hit.
 *  Parameters:
 *      page_number - the page number to look up in the TLB
 *  Returns:
 *      The corresponding frame number if found, or -1 if not found.
 */
 int tlb_lookup(tlb_t* tlb, uint32_t page_number) {
    int index = tlb_hash(page_number);
    tlb_hash_entry_t *entry = tlb->hashmap[index];

    // Check the hashed bucket
    while (entry != NULL) {
        // Retrieve the data from the node
        tlb_entry_t *data = get_entry_from_node(entry->node_to_data);
        if (data != NULL && data->page_number == page_number) {
            // Before returning, update via LRU.
            dll_remove_node(tlb->LRU, entry->node_to_data);
            entry->node_to_data = dll_append(tlb->LRU, data);

            return data->frame_number;
        }
        entry = entry->next_in_chain;
    }

    return TLB_MISS; // Not found
}

/** tlb_create_entry - Allocates and initialises a TLB entry.
 * Parameters:
 *     page_number - the logical page number
 *     frame_number - the physical frame number
 * Returns:
 *     Pointer to the newly created tlb_entry_t.
 */
 tlb_entry_t* tlb_create_entry(uint32_t page_number, uint32_t frame_number) {
    tlb_entry_t* entry = malloc(sizeof(tlb_entry_t));
    assert(entry != NULL);

    entry->page_number = page_number;
    entry->frame_number = frame_number;
    return entry;
}

/** tlb_add_to_hashmap - Inserts a dll_node containing a tlb_entry into the hashmap.
 * 
 * Parameters:
 *     tlb - pointer to the TLB structure
 *     page_number - the page number used for hashing
 *     node - pointer to the DLL node storing the tlb_entry
 */
 void tlb_add_to_hashmap(tlb_t* tlb, uint32_t page_number, dll_node_t* node) {
    int index = tlb_hash(page_number);

    // Create the new tlb_hash_entry for the page
    tlb_hash_entry_t *new_entry = malloc(sizeof(tlb_hash_entry_t));
    assert(new_entry != NULL);

    new_entry->node_to_data = node;
    new_entry->next_in_chain = tlb->hashmap[index];  
    tlb->hashmap[index] = new_entry;
}

/** tlb_add - Adds a new entry to the TLB if there is space.
 * Parameters:
 *     tlb - pointer to the TLB structure
 *     page_number - the logical page number
 *     frame_number - the corresponding physical frame number
 * Returns:
 *     1 on success, -1 on failure (i.e. full TLB).
 */
 int tlb_add(tlb_t* tlb, uint32_t page_number, uint32_t frame_number) {
    if (tlb->tlb_size >= TLB_NUMBER_OF_ENTRIES) {
        return TLB_FAIL;
    }

    tlb_entry_t* entry = tlb_create_entry(page_number, frame_number);
    assert(entry != NULL);

    dll_node_t* node = dll_append(tlb->LRU, entry);

    tlb_add_to_hashmap(tlb, page_number, node);
    tlb->tlb_size++;
    return TLB_SUCCESS;
}

/** tlb_flush_entry - Removes a TLB entry associated with the given page number.
 *  - Removes the entry from the TLB's hash map (with collision chaining accounted for),
 *  - Removes the corresponding node from the LRU list,
 *  - Frees all associated memory (hash entry and linked list node),
 *  - Decrements the TLB size.
 * Returns:
 *   the flushed page number if an entry was removed, otherwise returns -1.
 */
int32_t tlb_flush_entry(tlb_t *tlb, uint32_t page_number) {
    int index = tlb_hash(page_number);
    int flushed_page_number = TLB_FAIL;  // Used as a flag to determine if something was flushed
    
    // Find the entry in the hashmap at the computed index
    tlb_hash_entry_t* prev_entry = NULL;
    tlb_hash_entry_t* current_entry = tlb->hashmap[index];
    
    while (current_entry != NULL) {
        // Check if the current entry matches the page_number
        if (get_entry_from_node(current_entry->node_to_data)->page_number != page_number) {
            // Move to the next entry in the chain
            prev_entry = current_entry;
            current_entry = current_entry->next_in_chain;
            continue;
        } 
        // Match found — perform deletion
        
        // Get its data
        flushed_page_number = get_entry_from_node(current_entry->node_to_data)->page_number;

        // Remove the entry from the hashmap (chaining)
        if (prev_entry == NULL) {
            // This entry is the first in the chain
            tlb->hashmap[index] = current_entry->next_in_chain;
        } else {
            // This entry is in the middle or end of the chain
            prev_entry->next_in_chain = current_entry->next_in_chain;
        }

        // Remove the corresponding node from the LRU list
        tlb_free_entry(get_entry_from_node(current_entry->node_to_data));
        dll_remove_node(tlb->LRU, current_entry->node_to_data);
            // Implementation of dll_remove_node frees the node, but does 
            // not free the data in node
        current_entry->node_to_data = NULL;

        // Free the memory allocated for the tlb_hash_entry, which also 
        // frees tlb_entry_t (the data).
        tlb_free_hash_entry(current_entry);
        current_entry = NULL;

        // Decrement the TLB size
        tlb->tlb_size--;
    }
    
    return flushed_page_number; 
}


