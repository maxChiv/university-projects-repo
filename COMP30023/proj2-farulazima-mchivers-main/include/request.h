/*===========================================
request.h - COMP30023 Project2
    -> the interface for requests.c
Names: Max Chivers, Alif Farul Azim
Project: Web proxy
===========================================*/

#ifndef _REQUESTS_H_
#define _REQUESTS_H_

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#include "util/http_util.h"
#include "util/socket_util.h"

// Reasoning for buffer sizes:
// HOST_MAX_LEN: 256 bytes is typical max length for Host header per practical limits and RFCs (generous).
// RAW_REQUEST_MAX_LEN: 8 KB (8192 bytes) is a common upper bound for HTTP header size (RFC 7230 recommends servers limit header size).

#define HOST_MAX_LEN 256
#define RAW_REQUEST_MAX_LEN 8192

#define REQUEST_FAIL -1
#define REQUEST_SUCCESS 0

/* --- FORWARD DECLARATIONS --- */
typedef struct http_request http_request_t;

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
http_request_t *http_request_init(void);
void http_request_destroy(http_request_t *req);
// Parses an HTTP request from client_fd into req struct.
int parse_http_request(int client_fd, http_request_t *req);

const char *http_request_get_raw_request(const http_request_t *req);
size_t http_request_get_raw_request_len(const http_request_t *req);
const char *http_request_get_host(const http_request_t *req);
int http_request_equals(http_request_t *a, http_request_t *b);

#endif