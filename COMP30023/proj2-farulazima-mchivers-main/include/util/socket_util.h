/*===========================================
socket_utils.h - COMP30023 Project 2
    -> interface file for socket_utils.c
Names: Max Chivers, Alif Farul Azim
Project: Web Proxy
===========================================*/

#ifndef _SOCKET_UTIL_H_
#define _SOCKET_UTIL_H_

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <stdio.h>

#define BACKLOG 10
#define SOCKET_UTIL_FAIL -1
#define SOCKET_UTIL_SUCCESS 0

/* --- EXTERNAL FUNCTION PROTOTYPES --- */

// Accepts a new connection on a listener socket.
int accept_connection(int listener_fd);


// Sends all data from a buffer over the given socket.
int send_all(int fd, const char *buf, size_t len);

// Resolves a host and port into a list of addrinfo structs. Caller must free the result with freeaddrinfo(). 
int resolve_host(const char *host, const char *port, struct addrinfo **out);

// Connects to a host and port using TCP.
int connect_to_host(const char *host, const char *port);

// Sets up a server socket listening on the given port.
int prepare_server_socket(const char *port_str);

// Receives data from a socket into `buffer` until the specified `delimiter` is found.
int recv_until_delimiter(int fd, char *buffer, size_t max_len, const char *delimiter);

// Streams `length` bytes from `src_fd` to `dest_fd`, while also storing the streamed data
int stream_and_store(int src_fd, int dest_fd, ssize_t length, char **out_buf);

#endif 
