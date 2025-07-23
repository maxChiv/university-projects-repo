/*===========================================
response.c - COMP30023 Project 2
    -> Implements all functionality related to parsing HTTP response 
       from origin server
Names: Max Chivers, Alif Farul Azim
Project: Web Proxy
===========================================*/
#include "response.h"

/* --- INTERNAL FUNCTION PROTOTYPES --- */
void http_response_free(http_response_t *resp);

/* --- DEFINING STRUCTS --- */
struct http_response {
    char *headers;         // HTTP repsonse headers (malloc'd buffer)
    size_t headers_len;    // Length of the headers buffer
    char *body;            // HTTP repsonse body (malloc'd buffer)
    size_t body_len;       // Length of the body buffer
    uint32_t max_age;      // in seconds, 0 to UINT32_MAX
};

/* --- FUNCTION IMPLEMENTATIONS --- */

/**
 * Streams the response headers and body from the origin server to the client, 
 * and saves the headers and body of response to the struct.
 * 
 * The function first reads and forwards the response headers, then parses the
 * Content-Length field. It proceeds to stream the response body from origin_fd
 * to client_fd without buffering the full body in memory.
 * 
 * Returns:
 *   http_response_t with the information on success,
 *   STREAM_FAIL on any read/write error.
 */
http_response_t* stream_and_store_response(int origin_fd, int client_fd) {
    if (origin_fd < 0 || client_fd < 0) return STREAM_FAIL;

    // Need to find the Content-Length before sending anything, for security and certainty.

    // Retrieve the headers from response.
    char buffer[RESPONSE_HEADER_MAX];
    int buffer_len = recv_until_delimiter(origin_fd, buffer, RESPONSE_HEADER_MAX, "\r\n\r\n");
    
    // Check how much of the body has been read through
    if (buffer_len <= 0) {
        fprintf(stderr, "Failed to read response headers\n");
        return STREAM_FAIL;
    }
    
    // Find where the headers end and the body MAY start.
    int header_len = find_header_end(buffer, buffer_len);
    
    // Retrieve the content length
    ssize_t content_length = retrieve_content_length(buffer);
    if (content_length < 0) {
        fprintf(stderr, "Invalid or missing Content-Length\n");
        return STREAM_FAIL;
    }
    
    // Send headers to client_fd using send() with proper partial sends handling.
    if (send_all(client_fd, buffer, header_len) == SOCKET_UTIL_FAIL) {
        fprintf(stderr, "Failed to send headers to client\n");
        return STREAM_FAIL;
    }
    
    
    // Send inital body that may have been recv'd earlier, if any
    char* initial_body = buffer + header_len;
    ssize_t initial_body_len = buffer_len - header_len;
    if (initial_body_len > 0) {
        if (send_all(client_fd, initial_body, initial_body_len) == SOCKET_UTIL_FAIL) {
            fprintf(stderr, "Failed to send already-received body\n");
            return STREAM_FAIL;
        }
    }
    
    char* remaining_body = NULL;
    ssize_t remaining_body_len = content_length - initial_body_len;
    if (remaining_body_len != 0) {
        // HAVENT recv everything within first recv.
        // need to stream anything else. 
        // Stream remaining response body from origin_fd to client_fd, passing all the content we havent recv.
        if (stream_and_store(origin_fd, client_fd, remaining_body_len, &remaining_body) == SOCKET_UTIL_FAIL) {
            fprintf(stderr, "Failed to stream response body\n");
            return STREAM_FAIL;
        }
    }
    
    // NOW STORE THE INFORMATION TO RETURN 
    // Create the response struct to return
    http_response_t *resp = http_response_init();

    // SAVE THE HEADER TO THE STRUCTS
    char* headers = malloc(header_len);  // Allocate memory fir it.
    if (!headers) {
        fprintf(stderr, "malloc of headers");
        return STREAM_FAIL;
    }
    memcpy(headers, buffer, header_len);  
    // directly allocate to resp struct for storage
    resp->headers = headers;
    resp->headers_len = header_len;
    resp->max_age = get_max_age_from_headers(headers);

    // Combine inital and remaining body to make the entire body
    char *full_body = malloc(content_length);
    if (!full_body) {
        fprintf(stderr, "malloc of full body");
        free(remaining_body);
        free(headers);
        return STREAM_FAIL;
    }
    memcpy(full_body, initial_body, initial_body_len);  // add the inital part
    if (remaining_body_len > 0 && remaining_body != NULL) {
        // add the remaining part if necassary.
        memcpy(full_body + initial_body_len, remaining_body, remaining_body_len);  
    }
    
    // avoid memory leak
    if (remaining_body != NULL) {
        free(remaining_body);
    }

    // SAVE THE BODY TO THE STRUCT
    resp->body = full_body;
    resp->body_len = content_length;

    return resp;
}


/**
 * Allocates and initialises a new http_response_t struct with fields zeroed/NULL.
 */
http_response_t *http_response_init(void) {
    http_response_t *resp = malloc(sizeof(http_response_t));
    if (resp == NULL) {
        fprintf(stderr, "Malloc error: memory allocation for http_response_t failed\n");
        return NULL;
    }

    // Initialise all pointer fields to NULL and lengths to 0
    resp->headers = NULL;
    resp->headers_len = 0;
    resp->body = NULL;
    resp->body_len = 0;
    resp->max_age = NO_MAX_AGE;  // Assume no max age.

    return resp;
}


/**
 * Frees the internal fields of the http_response_t struct safely.
 */
void http_response_free(http_response_t *resp) {
    if (resp == NULL) return;

    if (resp->headers != NULL) {
        free(resp->headers);
        resp->headers = NULL;
        resp->headers_len = 0;
    }

    if (resp->body != NULL) {
        free(resp->body);
        resp->body = NULL;
        resp->body_len = 0;
    }
}


/**
 * Frees all allocated fields inside the http_response_t struct,
 * then frees the struct pointer itself.
 *
 * After calling this, the pointer passed in must not be used again (ASSIGN NULL!).
 */
void http_response_destroy(http_response_t *resp) {
    if (resp == NULL) return;

    // Free internal allocated fields
    http_response_free(resp);

    // Free the struct itself
    free(resp);
}


/* --- GET FUNCTIONS --- */
const char *http_response_get_headers(const http_response_t *resp) {
    if (!resp) return NULL;
    return resp->headers;
}

size_t http_response_get_headers_len(const http_response_t *resp) {
    if (!resp) return 0;
    return resp->headers_len;
}

const char *http_response_get_body(const http_response_t *resp) {
    if (!resp) return NULL;
    return resp->body;
}

size_t http_response_get_body_len(const http_response_t *resp) {
    if (!resp) return 0;
    return resp->body_len;
}

uint32_t http_response_get_max_age(const http_response_t* resp) {
    if (!resp) return NO_MAX_AGE;
    return resp->max_age;
}

/**
 * Sends the complete HTTP response (headers and body) over the given socket.
 * 
 *   fd       - Socket file descriptor to send data over
 *   response - Pointer to the http_response struct containing headers and body
 * 
 * Returns:
 *   RESPONSE_SUCCESS on success (all data sent),
 *  RESPONSE_FAIL on failure (if send_all fails)
 */
int send_http_response(int fd, const struct http_response *response) {
    if (fd < 0 || response == NULL) return RESPONSE_FAIL;

    // Send headers first
    if (response->headers_len > 0 && response->headers != NULL) {
        if (send_all(fd, response->headers, response->headers_len) == SOCKET_UTIL_FAIL) {
            return RESPONSE_FAIL;
        }
    }

    // Send body next
    if (response->body_len > 0 && response->body != NULL) {
        if (send_all(fd, response->body, response->body_len) == SOCKET_UTIL_FAIL) {
            return RESPONSE_FAIL;
        }
    }

    return RESPONSE_SUCCESS;
}