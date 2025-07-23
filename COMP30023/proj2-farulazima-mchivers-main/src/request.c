/*===========================================
request.c - COMP30023 Project 2
    -> Implements all functionality related to
       parsing HTTP GET requests
Names: Max Chivers, Alif Farul Azim
Project: Web Proxy
===========================================*/

#include "request.h"

/* --- INTERNAL FUNCTION PROTOTYPES --- */
void http_request_free(http_request_t *req);

/* --- DEFINING STRUCTS --- */
struct http_request {
    char *host;                // Extracted Host header value for TCP connect (malloc'd string)
    char *raw_request;         // Full raw HTTP request headers + request line (malloc'd buffer)
    size_t raw_request_len;    // Length of the raw_request buffer

};

/* --- FUNCTION IMPLEMENTATIONS --- */

/**
 * Allocates and initializes a new http_request_t struct with fields zeroed/NULL.
 */
http_request_t *http_request_init(void) {
    http_request_t *req = malloc(sizeof(http_request_t));
    if (req == NULL) {
        fprintf(stderr, "Malloc error: memory allocation failed\n");
        return NULL;
    }

    // Initialis all pointer fields to NULL and lengths to 0
    req->raw_request = NULL;
    req->raw_request_len = 0;
    req->host = NULL;
    return req;
}

/**
 * Frees all allocated fields inside the http_request_t struct,
 * then frees the struct pointer itself.
 *
 * After calling this, the pointer passed in must not be used again (ASSIGN NULL!).
 */
void http_request_destroy(http_request_t *req) {
    if (req == NULL) return;

    // Free internal allocated fields
    http_request_free(req);

    // Free the struct itself
    free(req);
}


/**
 * Parses an HTTP request from a client socket (client_fd) and populates
 * the given http_request_t structure.
 * 
 * This includes:
 * - Reading the request headers until the blank line (\r\n\r\n)
 * - Extracting the Host header
 * - Extracting the request line
 * - Extracting the path and origin-form version of the request line
 *      (this is due to using a proxy and will have absolute, not origin form)
 * - Saving the last header line (used for logging)
 * 
 * Returns:
 *   0 on success,
 *  REQUEST_FAIL on failure (e.g. malformed request, read errors).
 */
int parse_http_request(int client_fd, http_request_t *req) {
    if (req == NULL) return REQUEST_SUCCESS;

    // 1. Allocate a temporary buffer to read raw request first
    char temp_raw[RAW_REQUEST_MAX_LEN];
    int nread = recv_until_delimiter(client_fd, temp_raw, sizeof(temp_raw), "\r\n\r\n");

    if (nread <= 0 || nread >= (int)sizeof(temp_raw)) {
        fprintf(stderr, "Parsing HTTP error: failed to read raw request or buffer overflow\n");
        return REQUEST_FAIL;
    }

    // Allocate and copy raw_request dynamically
    req->raw_request = malloc(nread + 1);
    if (req->raw_request == NULL) return REQUEST_FAIL;
    memcpy(req->raw_request, temp_raw, nread);
    req->raw_request[nread] = '\0';
    req->raw_request_len = (size_t)nread;

    // 2. Extract host header (returns malloc'd string)
    char *host = extract_header_value(req->raw_request, HOST_HEADER_STR);
    if (!host) {
        fprintf(stderr, "Parsing HTTP error: Host header missing or malformed\n");
        http_request_free(req);
        return REQUEST_FAIL;
    }
    req->host = host;

    return REQUEST_SUCCESS; // Will not reach here if error exits
}

/**
 * Frees the http_request_t struct safely.
 */ 
void http_request_free(http_request_t *req) {
    if (req == NULL) return;

    if (req->raw_request != NULL) {
        free(req->raw_request);
        req->raw_request = NULL;
        req->raw_request_len = 0;
    }

    if (req->host != NULL) {
        free(req->host);
        req->host = NULL;
    }
}


/* --- GET FUNCTIONS --- */
const char *http_request_get_raw_request(const http_request_t *req) {
    if (!req) return NULL;
    return req->raw_request;
}

size_t http_request_get_raw_request_len(const http_request_t *req) {
    if (!req) return 0;
    return req->raw_request_len;
}

const char *http_request_get_host(const http_request_t *req) {
    if (!req) return NULL;
    return req->host;
}

/**
 * An equality checker for http requests that checks all aspects of the request
 */
int http_request_equals(http_request_t *a, http_request_t *b) {
    if (a == b) return REQUEST_SUCCESS;      // Same pointer or both NULL
    if (a == NULL || b == NULL) return REQUEST_FAIL;

    // Compare host strings (handle NULL cases)
    if (a->host == NULL && b->host != NULL) return REQUEST_FAIL;
    if (a->host != NULL && b->host == NULL) return REQUEST_FAIL;
    if (a->host != NULL && b->host != NULL) {
        if (strcmp(a->host, b->host) != 0) return REQUEST_FAIL;
    }

    // Compare raw_request lengths first
    if (a->raw_request_len != b->raw_request_len) return REQUEST_FAIL;

    // Compare raw_request buffers bytewise (handle NULL cases)
    if (a->raw_request == NULL && b->raw_request != NULL) return REQUEST_FAIL;
    if (a->raw_request != NULL && b->raw_request == NULL) return REQUEST_FAIL;
    if (a->raw_request != NULL && b->raw_request != NULL) {
        if (memcmp(a->raw_request, b->raw_request, a->raw_request_len) != 0) return REQUEST_FAIL;
    }

    return REQUEST_SUCCESS;
}