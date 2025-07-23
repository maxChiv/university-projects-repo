/*================================================================================
cache.c - COMP30023 Project2
    -> the implementation for caching in the proxy
Names: Max Chivers, Alif Farul Azim
Project: Web proxy

NOTES:
- this is based off my implementation of cache from project 1 and refactored for 
  project2

-   This implementation attempts to replicate a fast cache using a doubly-linked
    list for the LRU replacement policy, and a hashing function, to achieve 
    the below operation time complexities:
    
        Operation         Data Structure Used      Time Complexity
        ----------------------------------------------------------
        Lookup page          Hash                     O(1)
        Insert new page      List + Hash map          O(1)
        Evict page           Tail of list             O(1)
        Update usage (LRU)   Move node to head        O(1)
================================================================================*/

#include "cache.h"

/* --- INTERNAL FUNCTION PROTOTYPES --- */
cache_entry_t* get_entry_from_node(dll_node_t *node);
void cache_free_entry(void* entry);
int cache_hash(http_request_t* req);
void cache_free_hash_entry(cache_hash_entry_t *hash_entry);
cache_entry_t* cache_create_entry(http_request_t *req, http_response_t *resp);
cache_entry_t* cache_entry_lookup(cache_t* cache, http_request_t* req);


/* --- DEFINING STRUCTS --- */
struct cache_hash_entry {
    dll_node_t *node_to_data;                 // Points to the dll_node in the LRU which stores data.
    cache_hash_entry_t *next_in_chain;        // For collision chaining
};

struct cache_entry {
    http_request_t* req;    // HTTP REQUEST
    http_response_t* resp;  // HTTP RESPONSE 
    time_t time_accessed;   // Timestamp when entry was accessed (seconds since epoch)
};

struct cache {
    dll_list_t * LRU;                                      // LRU doubly linked list 
    cache_hash_entry_t* hashmap[CACHE_NUMBER_OF_ENTRIES];  // Hashmap to store pointers to LRU nodes
    uint32_t cache_size;  
};

/* --- FUNCTION IMPLEMENTATIONS --- */

/** cache_hash - computes the hash index for a given http_request_t for the cache
 * Returns:
 *  CACHE_FAIL on any issues with hash
 */
int cache_hash(http_request_t *req) {
    if (req == NULL) return CACHE_FAIL;
    // Retrieve the raw request
    const char* req_str = http_request_get_raw_request(req);
    if (req_str == NULL) return CACHE_FAIL;

    // Hash the request string using djb2 for strings.
    // djb2 hash
    uint64_t hash = 5381;
    int c;
    while ((c = *req_str++) != '\0') {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return (int)(hash % CACHE_NUMBER_OF_ENTRIES);
}

/** create_cache - allocates and initialises a new CACHE structure
 *  Returns:
 *      Pointer to the newly created CACHE, or NULL on allocation failure
 */
cache_t* cache_create(void) {
    cache_t *cache = malloc(sizeof(cache_t));
    if (cache == NULL) return NULL;

    cache->LRU = dll_list_create(); 
    if (cache->LRU == NULL) {
        free(cache);
        return NULL;
    }

    for (int i = 0; i < CACHE_NUMBER_OF_ENTRIES; i++) {
        cache->hashmap[i] = NULL;
    }

    cache->cache_size = 0;

    return cache;
}

/** cache_free_hash_entry - frees a hash entry and its associated data.
 * (Caller should allocate this hash_entry as NULL)
 */
void cache_free_hash_entry(cache_hash_entry_t *hash_entry) {
    if (hash_entry == NULL) return;

    // Free the LRU node and the cache entry inside it
    if (hash_entry->node_to_data != NULL) {
        dll_free_node(hash_entry->node_to_data, cache_free_entry);
    }
    // Free the hash entry itself
    free(hash_entry);
}

/** cache_free_entry - frees the allocated memory of the entry.
 *  - specifically used within the dll_list_t type.
 */
void cache_free_entry(void* entry) {
    if (entry == NULL) return;

    cache_entry_t* cache_entry = (cache_entry_t*)entry;

    http_request_destroy(cache_entry->req);
    http_response_destroy(cache_entry->resp);
    free(cache_entry);
}

/** cache_destroy - frees all memory used by the cache, including the LRU list
 *  and the hash map with any chained entries.
 * (caller should make this cache refrence NULL)
 */
 void cache_destroy(cache_t *cache) {
    if (cache == NULL) return;

    // Free all entries in the LRU list (this will call cache_free_entry on each node's data)
    dll_free_list(cache->LRU, cache_free_entry);

    // Free any chained entries in the hashmap
    for (int i = 0; i < CACHE_NUMBER_OF_ENTRIES; i++) {
        cache_hash_entry_t *entry = cache->hashmap[i];
        while (entry != NULL) {
            cache_hash_entry_t *next = entry->next_in_chain;
            free(entry);  // Already freed data in cache_free_entry
            entry = next;
        }
    }

    free(cache);
}

/** get_entry_from_node - extracts the CACHE entry stored in the given DLL node.
 *  Parameters:
 *      node - a pointer to the dll_node_t containing a CACHE entry
 *  Returns:
 *      A pointer to the cache_entry_t stored in the node.
 */
 cache_entry_t* get_entry_from_node(dll_node_t *node) {
    if (node == NULL) return NULL;
    return (cache_entry_t*)dll_get_data_ptr(node);
}

/** cache_get_LRU_request - Retrieves the least recently used request from the cache.
 */
http_request_t* cache_get_LRU_request(cache_t *cache) {
    return ((cache_entry_t*)dll_list_get_head(cache->LRU))->req;
}

/** cache_get_size - Retrieves the size of the cache.
 */
uint32_t cache_get_size(cache_t *cache) { return cache->cache_size; }

/** checks if the cache is full
 * returns: 
 * CACHE_SUCCESS (1) on full
 * CACHE_FAILL (-1) on not full 
*/
int cache_is_full(cache_t *cache) {
    if (cache == NULL) return CACHE_FAIL;  // treat NULL as fail
    if (cache_get_size(cache) >= CACHE_NUMBER_OF_ENTRIES) {
        return CACHE_SUCCESS;  // cache is full
    } else {
        return CACHE_FAIL;  // cache is not full
    }
}

/**
 * cache_lookup - searches for a request in the cache and returns its http response if found.
 *               Updates LRU order on hit.
 *
 * cache: pointer to the cache structure
 * req: pointer to the http_request_t to look up
 *
 * Returns: pointer to http_response if found, or CACHE_LOOKUP_MISS (NULL) if not found.
 */
 http_response_t* cache_lookup(cache_t* cache, http_request_t* req) {
    if (cache == NULL || req == NULL) return CACHE_LOOKUP_MISS;

    // Find correct hash_entry for updating
    int index = cache_hash(req);
    cache_hash_entry_t *hash_entry = cache->hashmap[index];
    // Find the actual cache entry (same as a call to get_entry_from_node(entry->node_to_data))
    cache_entry_t *cache_entry = cache_entry_lookup(cache, req);
    
    if (cache_entry != NULL) {
        /* Do not auto update the stale resposne to be removed so can be logged.
        if (is_response_stale(cache, req)) {
            // Response is stale so should be removed
            cache_flush_entry(cache, req);
            return CACHE_LOOKUP_MISS;
        }
        */   
        // Update LRU: move to most recently used
            // Remove from the hash entry from doubly-linked-list
            // Place at the front.
        dll_remove_node(cache->LRU, hash_entry->node_to_data);
        hash_entry->node_to_data = dll_append(cache->LRU, cache_entry);

        // Update the time stamp of access
        cache_entry->time_accessed = time(NULL);

        return cache_entry->resp;
    }

    return CACHE_LOOKUP_MISS; // Not found
}

/** cache_create_entry - Allocates and initializes a cache entry.
 * Parameters:
 *     req  - pointer to an HTTP request (assumed already allocated)
 *     resp - pointer to an HTTP response (assumed already allocated)
 * Returns:
 *     Pointer to the newly created cache_entry_t. OR NULL on fail
 */
cache_entry_t* cache_create_entry(http_request_t *req, http_response_t *resp) {
    cache_entry_t* entry = malloc(sizeof(cache_entry_t));
    if(entry == NULL) return NULL;

    entry->req = req;
    entry->resp = resp;

    // Initialize time_added to current time for freshness tracking
    entry->time_accessed = time(NULL);

    return entry;
}

/** cache_add_to_hashmap - Inserts a dll_node containing a cache_entry into the hashmap.
 * 
 * Parameters:
 *     cache - pointer to the cache structure
 *     req - the request used for hashing (same as in the node's data!!!)
 *     node - pointer to the DLL node storing the cache_entry
 */
 void cache_add_to_hashmap(cache_t* cache, http_request_t* req, dll_node_t* node) {
    int index = cache_hash(req);

    // Create the new cache_hash_entry for the page
    cache_hash_entry_t *new_entry = malloc(sizeof(cache_hash_entry_t));
    if (new_entry == NULL) return;  // MALLOC FAIL

    new_entry->node_to_data = node;
    new_entry->next_in_chain = cache->hashmap[index];  
    cache->hashmap[index] = new_entry;
}

/** cache_add - Adds a new entry to the cache if there is space.
 * Parameters:
 *     cache - pointer to the cache structure
 *     page_number - the logical page number
 *     frame_number - the corresponding physical frame number
 * Returns:
 *     CACHE_SUCCESS 1 on success, CACHE_FAIL -1 on failure (i.e. full cache).
 */
 int cache_add(cache_t* cache, http_request_t* req, http_response_t* resp) {
    if (cache_is_full(cache) == CACHE_SUCCESS) {
        return CACHE_FAIL;
    }

    cache_entry_t* entry = cache_create_entry(req, resp);
    if (entry == NULL) return CACHE_FAIL;

    dll_node_t* node = dll_append(cache->LRU, entry);

    //update from here 
    cache_add_to_hashmap(cache, req, node);
    cache->cache_size++;
    return CACHE_SUCCESS;
}

/**
 * cache_remove_entry - Removes a cache entry with the given page_number.
 *  - Removes from hashmap (handles chaining),
 *  - Removes from LRU list,
 *  - Frees all associated memory,
 *  - Decrements cache size.
 * Returns:
 *  The CACHE_SUCCESS (1) if removed, otherwise CACHE_FAIL (-1).
 */
int cache_flush_entry(cache_t *cache, http_request_t* req) {
    int index = cache_hash(req);
    int flush_flag = CACHE_FAIL;  // Used as a flag to determine if something was flushed
    
    // Find the entry in the hashmap at the computed index
    cache_hash_entry_t* prev_entry = NULL;
    cache_hash_entry_t* current_entry = cache->hashmap[index];
    
    while (current_entry != NULL) {
        // Check if the current entry matches the required request
        http_request_t* curr_req = get_entry_from_node(current_entry->node_to_data)->req;
        if (http_request_equals(curr_req, req) != REQUEST_SUCCESS) {
            // Move to the next entry in the chain
            prev_entry = current_entry;
            current_entry = current_entry->next_in_chain;
            continue;
        } 
        // Match found — perform deletion on curr_req

        // Remove the entry from the hashmap (chaining)
        if (prev_entry == NULL) {
            // This entry is the first in the chain
            cache->hashmap[index] = current_entry->next_in_chain;
        } else {
            // This entry is in the middle or end of the chain
            prev_entry->next_in_chain = current_entry->next_in_chain;
        }

        // Remove the corresponding node from the LRU list
        cache_free_entry(get_entry_from_node(current_entry->node_to_data));
        dll_remove_node(cache->LRU, current_entry->node_to_data);
            // Implementation of dll_remove_node frees the node, but does 
            // not free the data in node
        current_entry->node_to_data = NULL;

        // Free the memory allocated for the cache_hash_entry, which also 
        // frees cache_entry_t (the data).
        cache_free_hash_entry(current_entry);
        current_entry = NULL;

        // Decrement the TLB size
        cache->cache_size--;
        flush_flag = CACHE_SUCCESS;
    }
    
    return flush_flag; 
}


/** cache_evict_LRU - Evicts the least recently used entry from the cache.
 * 
 * This function identifies the least recently used (LRU) cache entry and
 * removes it using the cache_flush_entry function. It ensures the LRU list
 * and underlying cache structures remain consistent after eviction.
 
 * Returns:
 *     CACHE_SUCCESS (1) on successful eviction,
 *     CACHE_FAIL (-1) if the cache is NULL, the LRU list is empty,
 *     or if eviction fails.
 */
int cache_evict_LRU(cache_t *cache) {
    if (cache == NULL || cache->LRU == NULL) return CACHE_FAIL;

    // Get the LRU request
    http_request_t* req = cache_get_LRU_request(cache);
    if(req == NULL) return CACHE_FAIL;

    return cache_flush_entry(cache, req);
}

/*
 * Arguments
 * resp -> The response from which to extract the value of the cache-control header
 * 
 * Utilises http_util function by similar name.
 * 
 * Returns an IS_CACHABLE_FALSE 0 if the conditions for caching are not met
 * Returns an CACHABLE_TRUE 1 if the conditions for caching are met
 *      This is also assumed if the Cache-control is not found.
 * 
 * Conditions checked for caching 
 * 1) Check for the cache-control arguments specified in task 3(private, no-store, no-cache, max-age=0,must-revalidate, proxy-revalidate)
*/
int is_cacheable_response(http_response_t* resp) {
    return is_cacheable(http_response_get_headers(resp));
}


/**
 * Find the full cache entry.
 * This does not do any updates and is used to only retrieve the cache entry.
 * Returns:
 *  A cache_entry_t if the req is in the cache, or NULL.
 */
cache_entry_t* cache_entry_lookup(cache_t* cache, http_request_t* req) {
    if (cache == NULL || req == NULL) return NULL;

    // Find the entry in the hashmap at the computed index
    int index = cache_hash(req);
    cache_hash_entry_t *entry = cache->hashmap[index];

    while (entry != NULL) {
        // Retrieve the data from the node, and checking 
        // chaining to make sure its the correct cache entry.
        cache_entry_t *data = get_entry_from_node(entry->node_to_data);
        if (data != NULL && http_request_equals(data->req, req) == REQUEST_SUCCESS) {
            return data; // return full cache entry
        }
        entry = entry->next_in_chain;
    }

    return NULL; // not found
}

/**
 * Checks if the cached response for the given request is stale.
 *
 * return: 
 *         CACHE_STALE (1) if the response is stale ,
 *         CACHE_NOT_STALE (0) if the response is fresh (not stale)or no cached response exists.
 */
int is_response_stale(cache_t *cache, http_request_t *req) {
    cache_entry_t *entry = cache_entry_lookup(cache, req);
    if (entry == NULL || entry->resp == NULL) return CACHE_NOT_STALE;

    uint32_t max_age = http_response_get_max_age(entry->resp);
    // printf("====\nCHECKS FOR AGE: max: %d, delta time: %d\n", max_age, (uint32_t)(time(NULL) - entry->time_added)); // Checks the time passed
    if (max_age == NO_MAX_AGE) return CACHE_NOT_STALE;

    time_t now = time(NULL);
    if ((uint32_t)(now - entry->time_accessed) > max_age) {
        return CACHE_STALE;
    }

    return CACHE_NOT_STALE;
}


/**
 * Replace the cached response for a given request.
 * If the request is in the cache, its response is replaced and the old one is freed.
 * If the request is not in the cache, nothing is done.
 *
 *     cache - pointer to the cache structure
 *     req   - the request to update
 *     resp  - the new response to store
 *
 * Returns:
 *     CACHE_SUCCESS (1) if replaced successfully,
 *     CACHE_FAIL (-1) if entry was not found or parameters were invalid.
 */
int cache_replace_response(cache_t *cache, http_request_t *req, http_response_t *resp) {
    if (cache == NULL || req == NULL || resp == NULL) {
        return CACHE_FAIL;
    }

    cache_entry_t *entry = cache_entry_lookup(cache, req);
    if (entry == NULL) {
        return CACHE_FAIL;
    }

    if (entry->resp != NULL) {
        http_response_destroy(entry->resp);
    }

    entry->resp = resp;  // update to the new resp
    entry->time_accessed = time(NULL);  // reset time to now for new response

    return CACHE_SUCCESS;
}
