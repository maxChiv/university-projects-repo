/*===========================================
proxy.h - COMP30023 Project2
    -> the interface for proxy.c
Names: Max Chivers, Alif Farul Azim
Project: Web proxy
===========================================*/

#ifndef _PROXY_H_
#define _PROXY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "response.h"
#include "request.h"
#include "cache.h"
#include "util/socket_util.h"
#include "util/http_util.h"

#define BACKLOG 10
#define HOST_PORT_STR "80"

#define MAX_REQUEST_LEN 2000     // 2000 byte limit
#define MAX_RESPONSE_LEN 102400  // 100 KiB limit = 100 × 1024 bytes

/* --- EXTERNAL FUNCTION PROTOTYPES --- */
void run_proxy(int port, int cache_status);
void proxy_cleanup();

#endif