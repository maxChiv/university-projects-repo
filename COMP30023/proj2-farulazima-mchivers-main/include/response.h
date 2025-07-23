/*===========================================
response.h - COMP30023 Project2
    -> the interface for response.c
Names: Max Chivers, Alif Farul Azim
Project: Web proxy
===========================================*/
#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <stdint.h>

#include "util/http_util.h"
#include "util/socket_util.h"

#define RESPONSE_FAIL -1
#define RESPONSE_SUCCESS 1
#define STREAM_FAIL (NULL)
#define RESPONSE_HEADER_MAX 8192  // Max size of HTTP headers (8 KiB is common)

/* --- FORWARD DECLARATIONS --- */
typedef struct http_response http_response_t;

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
http_response_t* stream_and_store_response(int origin_fd, int client_fd);
    // For data struct:
http_response_t *http_response_init(void);
void http_response_destroy(http_response_t *resp);
    // get functions
const char *http_response_get_headers(const http_response_t *resp);
size_t http_response_get_headers_len(const http_response_t *resp);
uint32_t http_response_get_max_age(const http_response_t* resp);


const char *http_response_get_body(const http_response_t *resp);
size_t http_response_get_body_len(const http_response_t *resp);

int send_http_response(int fd, const struct http_response *response);

#endif