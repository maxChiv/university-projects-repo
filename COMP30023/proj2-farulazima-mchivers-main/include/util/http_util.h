/*===========================================
http_utils.h - COMP30023 Project 2
    -> interface file for http_utils.c
Names: Max Chivers, Alif Farul Azim
Project: Web Proxy
===========================================*/

#ifndef _HTTP_UTILS_H_
#define _HTTP_UTILS_H_

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>

#define HTTP_UTIL_SUCCESS 0
#define HTTP_UTIL_FAIL -1
#define TEMP_BUF_SIZE 512

#define IS_CACHEABLE_TRUE 1
#define IS_CACHEABLE_FALSE 0

#define HOST_HEADER_STR "Host"
#define CONTENT_LENGTH_HEADER_STR "Content-Length"

#define NO_MAX_AGE UINT32_MAX

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
// Extracts the value of a given header from the headers string.
char *extract_header_value(const char *headers, const char *header_name);

// Finds the end of HTTP headers (index after CRLF CRLF).
int find_header_end(const char *buffer, size_t len);

// Returns pointer to start of body in raw HTTP request.
char *retrieve_header_tail(const char *raw_request);

// Retrieves Content-Length value from headers.
ssize_t retrieve_content_length(const char *headers);

// Extracts the full request URI from a raw HTTP request.
char *retrieve_request_uri(const char *raw_request);

// Checks if a response is cacheable based on headers.
int is_cacheable(const char *resp_headers);

// Extracts max-age (in seconds) from Cache-Control header.
uint32_t get_max_age_from_headers(const char *headers);


#endif