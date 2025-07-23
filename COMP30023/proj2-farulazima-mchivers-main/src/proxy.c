/*===========================================
proxy.c - COMP30023 Project2
    -> the overall proxy logic, utilising all other modules. 
Names: Max Chivers, Alif Farul Azim
Project: Web proxy

NOTES:
The overall logical flow of the proxy goes
run_proxy()
  └─> handle_client_connection(cache, client_fd, cache_status)
         └─> handle_http_request(cache, client_fd, req, cache_status, is_cacheable)
                ├─> handle_cache_lookup(cache, req, &resp)  (if cached)
                ├─> handle_send_request_to_origin(origin_fd, req)
                ├─> stream_and_store_response(origin_fd, client_fd)
                ├─> handle_cache_store(cache, req, resp)
                └─> handle_cache_size(cache)

Log Flow:
REQUEST
├── LOG: "Accepted"
├── LOG: "Request tail last line"
└── Cache Lookup
    ├── Hit
    │   ├── Fresh
    │   │   └── LOG: "Serving host request-URI from cache"
    │   └── Stale
    │       ├── LOG: "Stale entry for host request-URI"
    │       ├── LOG: "GETting host request-URI"
    │       ├── LOG: "Response body length content-length"
    │       └── Cache Handling
    │           ├── Not cacheable
    │           │   ├── LOG: "Not caching host request-URI"
    │           │   OR
    │           │   ├── Too big → LOG: "Evicting host request-URI from cache"
    │           └── Cacheable
    │               ├── **New will replace the stale cache entry**
    └── Miss
        ├── LOG: "GETting host request-URI"
        ├── LOG: "Response body length content-length"
        └── Cache Handling
            ├── Not cacheable → LOG: "Not caching host request-URI"
            └── Cacheable
                ├── If full → LOG: "Evicting host request-URI from cache"

===========================================*/

#include "proxy.h"

#define HANDLE_FAIL -1
#define HANDLE_OK 0
#define HANDLE_SUCCESS 1

#define HANDLE_CACHE_HIT 1
#define HANDLE_CACHE_MISS 0

/* --- INTERNAL FUNCTION PROTOTYPES --- */
// Handles a new client connection and processes the HTTP request.
int handle_client_connection(cache_t* cache, int client_fd, int cache_status);

// Processes an HTTP request and determines how to respond.
int handle_http_request(cache_t* cache, int client_fd, http_request_t *req, int cache_status);

// Looks up a request in the cache and sets the response output if found.
int handle_cache_lookup(cache_t* cache, http_request_t *req, http_response_t **resp_out);

// Forwards the HTTP request to the origin server.
int handle_send_request_to_origin(int origin_fd, http_request_t *req);

// Stores the response in the cache if it's cacheable.
void handle_cache_store(cache_t* cache, http_request_t *req, http_response_t *resp);

// Manages the current cache size.
void handle_cache_size(cache_t *cache);


/* --- DEFINING STRUCTS --- */
struct http_response {
    char *headers;         // HTTP repsonse headers (malloc'd buffer)
    size_t headers_len;    // Length of the headers buffer
    char *body;            // HTTP repsonse body (malloc'd buffer)
    size_t body_len;       // Length of the body buffer
};

static cache_t *global_cache = NULL; // accessible by proxy_cleanup

void run_proxy(int port, int cache_status) {
    // Attempt to set up the proxy socket.
    char port_str[6];  // Enough for "65535\0"
    snprintf(port_str, sizeof(port_str), "%d", port);
    int proxy_fd = prepare_server_socket(port_str);
    if (proxy_fd == SOCKET_UTIL_FAIL) {
        fprintf(stderr, "Couldn't bind the proxy.\n");
        return;  // Couldn't bind so exit gracefully
    }
    
    // Init a cache
    global_cache = cache_create(); // Set the global/static one

    // Loop through with a backlog  for client connections 
    while (1) {
        // Connect to a client, if invalid, go to next client. 
        int client_fd = accept_connection(proxy_fd);
        if (client_fd == SOCKET_UTIL_FAIL) continue;

        // LOG 1
        printf("Accepted\n");
        fflush(stdout);

        if (handle_client_connection(global_cache, client_fd, cache_status) == HANDLE_FAIL  ) {
            fprintf(stderr, "Client handling failed\n");
        }

        close(client_fd);
    }
}

int handle_client_connection(cache_t* cache, int client_fd, int cache_status) {
    // parse the request from client 
    http_request_t *req = http_request_init();
    // For any unexpected behaviour, get next client. by returning -1
    if (req == NULL) {
            fprintf(stderr, "req malloc error");
            return HANDLE_FAIL;
    }
    if (parse_http_request(client_fd, req) == REQUEST_FAIL) {
        fprintf(stderr, "Client request parsing error");
        http_request_destroy(req);
        return HANDLE_FAIL;
    }
    
    // LOG 2
    char* req_tail = retrieve_header_tail(http_request_get_raw_request(req));
    printf("Request tail %s\n", req_tail);
    fflush(stdout);
    free(req_tail);  // malloc'd so must free.

    // To cache or not to cache
    // Returns 1 only if the function is cacheable

    // Forward and stream the response with possible caching, ie handle the request
    int result = handle_http_request(cache, client_fd, req, cache_status);

        // req free'd inside the handle
    return result;
}

int handle_http_request(cache_t* cache, int client_fd, http_request_t *req, int cache_status) {
    http_response_t *resp = NULL;

    int req_len = http_request_get_raw_request_len(req);

    if ((req_len < MAX_REQUEST_LEN) && cache_status) {
        // Check for cache (not implemented yet)
        if (handle_cache_lookup(cache, req, &resp) == HANDLE_CACHE_HIT) {
            // Cache hit with resp.
            
            // LOG 3
            char* req_uri = retrieve_request_uri(http_request_get_raw_request(req));
            printf("Serving %s %s from cache\n", http_request_get_host(req), req_uri);
            fflush(stdout);
            free(req_uri);  // malloc'd so must free.
            
            // Send the response
            send_http_response(client_fd, resp);
            
            // free request as it is no longer needed
            http_request_destroy(req);
            return HANDLE_OK;
        }
    }

    // Request too big, or Cache miss, so continue.
    // We only need to evict a form if we have to cache
    // The problem is that for task 3 we want it to only do it if cache control is enabled OR its task 2 and cache control header is non existent
    // This only handles the cache if cache-control allows it OR if its task 2 and no cache-control header is passed
    if (cache_status && is_response_stale(cache, req) == CACHE_NOT_STALE) {
        // Can handle the cache size, otherwise stale will be replaced or evicted. (delta time between calls to function is neglible)
        // Check if we need to evict a page for this request
        handle_cache_size(cache);
    }

    // LOG 5
    char* req_uri = retrieve_request_uri(http_request_get_raw_request(req));
    printf("GETting %s %s\n", http_request_get_host(req), req_uri);
    fflush(stdout);
    free(req_uri);  // malloc'd so must free.
    
    // Connect to origin
    int origin_fd = connect_to_host(http_request_get_host(req), HOST_PORT_STR);
    if (origin_fd == SOCKET_UTIL_FAIL) {
        return HANDLE_FAIL;
    }
    // Send request to origin
    if (handle_send_request_to_origin(origin_fd, req) == HANDLE_FAIL) {
        close(origin_fd);
        return HANDLE_FAIL;
    }
    
    // Stream and store response
    resp = stream_and_store_response(origin_fd, client_fd);
    if (resp == STREAM_FAIL) {
        close(origin_fd);
        return HANDLE_FAIL;
    }

    // LOG 6
    printf("Response body length %ld\n", http_response_get_body_len(resp));
    fflush(stdout);

    // Check whether there are non cacheable Cache-Control's 
    int is_cacheable_flag = is_cacheable_response(resp);
    if(cache_status &&  is_cacheable_flag == IS_CACHEABLE_FALSE) {
        // LOG 7
        char* req_uri = retrieve_request_uri(http_request_get_raw_request(req));
        printf("Not caching %s %s\n", http_request_get_host(req), req_uri);
        fflush(stdout);
        free(req_uri);  // malloc'd so must free.
        
        // Free everything then return.
        http_request_destroy(req);
        http_response_destroy(resp);
        return HANDLE_OK;
    }

    // Figure out the total length of the response.
    int resp_total_len = http_response_get_headers_len(resp) 
                         + http_response_get_body_len(resp);
    
    if ((req_len < MAX_REQUEST_LEN && resp_total_len <= MAX_RESPONSE_LEN && cache_status && is_cacheable_flag) ||
        (req_len < MAX_REQUEST_LEN && resp_total_len <= MAX_RESPONSE_LEN && cache_status)) {
        handle_cache_store(cache, req, resp); 
    } else {
        if(is_response_stale(cache, req)) {
            // Doesnt fit anymore and not a valid cache but still within the cache so evict.
            // LOG 7
            char *req_uri = retrieve_request_uri(http_request_get_raw_request(req));
            printf("Evicting %s %s from cache\n", http_request_get_host(req), req_uri);
            fflush(stdout);
            free(req_uri);
            cache_flush_entry(cache, req);
        }
        // Free memory so no leaks as long as it wasnt added to cache.
        http_request_destroy(req);
        http_response_destroy(resp);
    }
    close(origin_fd);
    return HANDLE_OK;
}

/* returns 1 on success, 0 on fail */
int handle_cache_lookup(cache_t* cache, http_request_t *req, http_response_t **resp_out) {
    if (req == NULL || resp_out == NULL) return HANDLE_FAIL;

    // Check if the request is stale.
        // A static check which will not update anything and 
        // purely for logical and loggin purposes
    if(is_response_stale(cache, req)) {
        // LOG 4
        char *req_uri = retrieve_request_uri(http_request_get_raw_request(req));
        printf("Stale entry for %s %s\n", http_request_get_host(req), req_uri);
        fflush(stdout);
        free(req_uri);
        return HANDLE_CACHE_MISS;    
    }
    
    // Attempt a cache_lookup
        // Automatically updates and will also return a miss if it's stale
    http_response_t *cached_resp = cache_lookup(cache, req); 
    
    // Return on success
    if (cached_resp != NULL) {
        *resp_out = cached_resp;
        return HANDLE_CACHE_HIT;  // cache hit
    }

    return HANDLE_CACHE_MISS;  // cache miss
}


int handle_send_request_to_origin(int origin_fd, http_request_t *req) {
    // Send everything to the origin
    const char *buf = http_request_get_raw_request(req);
    size_t len = http_request_get_raw_request_len(req);
    if (send_all(origin_fd, buf, len) == SOCKET_UTIL_FAIL) {
        // Failed to send everything.
        fprintf(stderr, "Send request to origin failed\n");
        return HANDLE_FAIL;
    }
    return HANDLE_OK;  // Success, sent everything 
}


void handle_cache_store(cache_t* cache, http_request_t *req, http_response_t *resp) {
    if (cache == NULL || req == NULL || resp == NULL) return;

    // Handle a stale replacing itself
    if(is_response_stale(cache, req)) {
        cache_flush_entry(cache, req);
    }
    
    // SHOULD NOT BE FULL SINCE EVICTED EARLIER
    // Add the new request-response pair to the cache
    cache_add(cache, req, resp);
}


/**
 * Checks if the cache is full and evicts the least recently used (LRU) entry if needed.
 */
void handle_cache_size(cache_t *cache) {
    if (cache == NULL) return;

    if (cache_is_full(cache) == CACHE_SUCCESS) {
        http_request_t *req_LRU = cache_get_LRU_request(cache);
        if (req_LRU != NULL) {
            // LOG 7
            char *req_uri = retrieve_request_uri(http_request_get_raw_request(req_LRU));
            printf("Evicting %s %s from cache\n", http_request_get_host(req_LRU), req_uri);
            fflush(stdout);
            free(req_uri);
        }

        cache_evict_LRU(cache);
    }
}

/* USED AS A CLEANUP ON A SIGINT */
void proxy_cleanup() {
    if (global_cache != NULL) {
        cache_destroy(global_cache);
        global_cache = NULL;
        // fprintf(stderr, "Proxy resources cleaned up.\n");
    }
}