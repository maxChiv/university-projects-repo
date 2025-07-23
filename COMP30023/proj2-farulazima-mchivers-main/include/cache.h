/*===========================================
cache.h - COMP30023 Project2
    -> the interface for cache.c
Names: Max Chivers, Alif Farul Azim
Project: Web proxy

NOTE:
- this is based off my implementation of cache
  from project 1
===========================================*/
#ifndef _CACHE_H_
#define _CACHE_H_

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "linked-list/generic-doubly-linkedlist.h"
#include "linked-list/linkedlist.h"
#include "request.h"
#include "response.h"
#include "util/http_util.h"

#define CACHE_FAIL -1
#define CACHE_SUCCESS 1
#define CACHE_LOOKUP_MISS (NULL)
#define CACHE_NUMBER_OF_ENTRIES 10 /* Number of entries for the cache */
#define CACHE_STALE 1
#define CACHE_NOT_STALE 0

/* --- FORWARD DECLARATIONS --- */
typedef struct cache_entry cache_entry_t;
typedef struct cache cache_t;
typedef struct cache_hash_entry cache_hash_entry_t;

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
// Creates and returns a new cache instance.
cache_t* cache_create(void);

// Frees all memory associated with the cache.
void cache_destroy(cache_t *cache);

// Looks up a request in the cache and returns the stored response.
http_response_t* cache_lookup(cache_t* cache, http_request_t* req);

// Adds a new request-response pair to the cache.
int cache_add(cache_t* cache, http_request_t* req, http_response_t* resp);

// Removes a specific entry from the cache.
int cache_flush_entry(cache_t *cache, http_request_t* req);

// Returns the least recently used request in the cache.
http_request_t* cache_get_LRU_request(cache_t *cache);

// Returns the current size of the cache.
uint32_t cache_get_size(cache_t *cache);

// Checks if the cache is full.
int cache_is_full(cache_t *cache);

// Removes the least recently used entry from the cache.
int cache_evict_LRU(cache_t *cache);

// Checks if a response is eligible to be cached.
int is_cacheable_response(http_response_t* resp);

// Checks if a cached response is stale.
int is_response_stale(cache_t *cache, http_request_t *req);

// Replaces the cached response for an existing request.
int cache_replace_response(cache_t *cache, http_request_t *req, http_response_t *resp);

#endif