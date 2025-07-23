/*===========================================
socket_utils.c - COMP30023 Project 2
    -> socket utilities module for raw socket 
       operations.
Names: Max Chivers, Alif Farul Azim
Project: Web Proxy
===========================================*/

#include "util/socket_util.h"

#define TEMP_BUF_SIZE 512


/* --- INTERNAL FUNCTION PROTOTYPES --- */

int create_socket();
int bind_and_listen(struct addrinfo *addr_list, int backlog);

/* --- FUNCTION IMPLEMENTATIONS --- */
/**
 * Creates a socket using the specified address family, socket type, and protocol.
 * 
 * Wrapper for `socket()`. Returns a valid file descriptor
 * or SOCKET_UTIL_FAILon error.
 * 
 * Returns:
 *   Socket file descriptor on success,
 *   SOCKET_UTIL_FAILon failure .
 */
int create_socket() {
    int sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);  // Maybe #define these later since i dont see it changing;
    if (sockfd == -1) {
        fprintf(stderr, "create_socket: socket creation failed");
        return SOCKET_UTIL_FAIL;
    }

    return sockfd;
}

/**
 * Accepts a new client connection on a listener socket.
 * 
 * This function checks the list of pending requests to connect to that socket and connects to the first one in the queue
 * It then returns the next completed connection from the front of the completed connection queue
 * accept() will get the address and the length of this address from the peer 
 * 
 * Note: Doesnt car abotu any client information 
 * 
 * Returns:
 *   Client socket file descriptor on success,
 *   SOCKET_UTIL_FAIL on failure .
 */
int accept_connection(int listener_fd) {
    // Declare storage for the client info. 
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    int client_fd = accept(listener_fd, (struct sockaddr *)&addr, &len);

    if (client_fd < 0) {
        fprintf(stderr, "accept() has failed. Failed to accept request");
        return SOCKET_UTIL_FAIL;
    }

    return client_fd;
}

/**
 * Sends the full buffer `buf` of size `len` over the socket `fd`.
 * 
 * Retries until all bytes are sent or an error occurs. Intended for use when
 * partial sends must be avoided.
 * 
 * Returns:
 *   SOCKET_UTIL_SUCCESS on success (all bytes sent),
 *   SOCKET_UTIL_FAILon failure .
 */
int send_all(int fd, const char *buf, size_t len) {
    if (fd < 0 || buf == NULL) return SOCKET_UTIL_FAIL;

    size_t total_sent = 0;       
    ssize_t bytes_sent;

    // Send with proper partial sends to ensure everything is sent to fd
    while (total_sent < len) {
        bytes_sent = send(fd, buf + total_sent, len - total_sent, 0);

        if (bytes_sent <= 0) {
            fprintf(stderr, "send returned <= 0 unexpectedly\n");
            return SOCKET_UTIL_FAIL;
        }

        // Possible partial send, then continue sending
        total_sent += bytes_sent;
    }

    return SOCKET_UTIL_SUCCESS;  // All bytes sent successfully
}

/**
 * Resolves a hostname and port into a list of addrinfo structs.
 * 
 * - Wrapper around `getaddrinfo()` with default IPv4 or IPv6, SOCK_STREAM hints.
 * - Caller must call `freeaddrinfo()` on *out when done.
 * - host can be null.
 * 
 * Returns:
 *   0 on success (*out is set),
 *   non-zero error code on failure (see getaddrinfo()).
 */
int resolve_host(const char *host, const char *port, struct addrinfo **res) {
    if (res == NULL) return SOCKET_UTIL_FAIL; 

    struct addrinfo hints;
    
    // Set up hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;   // TCP stream socket

    // Get addrinfo for binding
    int status = getaddrinfo(host, port, &hints, res);
    if (status != 0) {
        // getaddrinfo failed, return the error code (non-zero)
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return status;
    }

    // Success, *res is set to the result
    return 0;
}

/*
 * Before a socket can be used it first has to be accessible, by binding it to a port we give it an address
 * Arguments:
 * 1) File descriptor (This is given to use when we initialise a socket)
 * 2) A sockaddr object/structure that contains the address to be bound to the socket ()
 * 3) socklen_t -> this is just an unsigned int 32. This specifies the length of the address structure pointed to by the second argument
*/
void bind_socket_to_port(int file_descriptor,struct sockaddr* server_sockaddress, socklen_t length_of_sockaddr){
    // In order for bind to work it needs
    // 1) The file descriptor (This is given by the socket)
    // 2) The sockaddr structure contraining the address that we want to bound to this socket
    // 3) The length of the socket address structure

    int enable = 1;

    if (setsockopt(file_descriptor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        fprintf(stderr, "setsockopt error");
        //exit(1);
    }

    if(bind(file_descriptor, server_sockaddress, length_of_sockaddr) < 0){
        fprintf(stderr, "Bind() has failed\n");
        exit(1);  // Exit with status 1 on failure
    }

}

/**
 *  * --- a server function ---
 * Binds and listens on one of the provided addrinfo options.
 * 
 * Loops over `addr_list` until `bind()` and `listen()` succeed.
 * Automatically sets `SO_REUSEADDR` before binding.
 * 
 * Returns:
 *   Listener socket fd on success,
 *   SOCKET_UTIL_FAILon failure 
 */
int bind_and_listen(struct addrinfo *addr_list, int backlog) {
    struct addrinfo *rp;
    int sockfd;
    
    // Try each addr until we successfully bind
    for (rp = addr_list; rp != NULL; rp = rp->ai_next) {
        // Create socket with the address family, socktype, protocol
        sockfd = create_socket();

        if (sockfd == SOCKET_UTIL_FAIL) {
            fprintf(stderr, "socket");
            continue;
        }

        // Attempt to bind using provided helper function
        bind_socket_to_port(sockfd, rp->ai_addr, rp->ai_addrlen);

        // Start listening on the socket
        if (listen(sockfd, backlog) < 0) {
            close(sockfd);
            continue;
        }

        // Successfully bound and listening
        return sockfd;
    }

    // If we reach here, no bind+listen succeeded
    fprintf(stderr, "Could not bind and listen on any address\n");
    return SOCKET_UTIL_FAIL;
}

/**
 *  * --- a server function ---
 * Prepares a server socket that is bound and listening on the given port.
 * 
 * High-level wrapper that:
 *   - Resolves the given port into addrinfo,
 *   - Binds and listens on an available socket,
 *   - Cleans up addrinfo before returning.
 * 
 * Returns:
 *   Listener socket fd on success,
 *   SOCKET_UTIL_FAILon failure.
 */
int prepare_server_socket(const char *port_str) {
    struct addrinfo *res = NULL;
    int status;
    
    // Resolve the port into address info
    status = resolve_host(NULL, port_str, &res);
    if (status != 0) {
        fprintf(stderr, "Failed to resolve port %s\n", port_str);
        return SOCKET_UTIL_FAIL;
    }

    // Try to bind and listen on a valid address
    int sockfd = bind_and_listen(res, BACKLOG);

    // Clean up address info regardless of success/failure
    freeaddrinfo(res);

    return sockfd;
}


/** 
 * --- a client function ---
 * Connects to a remote host at the given hostname and port (used by a 
 * client to connect to a remote server).
 * 
 * Tries each addrinfo from `resolve_host()` until a connection succeeds.
 * 
 * Returns:
 *   Connected socket fd on success,
 *   SOCKET_UTIL_FAILon failure.
 */
int connect_to_host(const char *host, const char *port) {
    struct addrinfo *res, *rp;
    int sockfd = -1;

    // Resolve the hostname and port
    int status = resolve_host(host, port, &res);
    if (status != 0) {
        fprintf(stderr, "Failed to resolve host %s:%s: %s\n", host, port, gai_strerror(status));
        return SOCKET_UTIL_FAIL;
    }

    // Try each result until we successfully connect
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            continue;
        }

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            // Success
            break;
        }

        // If it doesnt work then we close this socket and try next in list.
        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(res);

    // No connection could be made
    if (rp == NULL) {
        fprintf(stderr, "Could not connect to host %s:%s\n", host, port);
        return SOCKET_UTIL_FAIL;
    }

    return sockfd;
}


/**
 * Receives data from a socket into `buffer` until the specified `delimiter` is found.
 * 
 * Parameters:
 *   - `fd`: Socket file descriptor to read from.
 *   - `buffer`: Destination buffer to fill with received data.
 *   - `max_len`: Maximum number of bytes to read (including space for null terminator).
 *   - `delimiter`: Null-terminated string indicating the stop condition (e.g., "\r\n\r\n").
 * 
 * Notes:
 *   - Buffer will be null-terminated.
 *   - Stops reading when the delimiter is found or when the buffer is full.
 *      - A possibility that there has been more read into the buffer after the delimter.
 *        As the function only returns once the delimeter is inside the buffer. 
 *   - Designed for use with blocking sockets or properly handled non-blocking reads.
 * 
 * Warnings:
 *   - Could yield some undefined behaviour with binary data being recv.
 * 
 * Returns:
 *   - Number of bytes read (including the delimiter) if successful.
 *   - SOKET_UTIL_FAIL (-1) on error or if delimiter is not found in available space.
 */
int recv_until_delimiter(int fd, char *buffer, size_t max_len, const char *delimiter) {
    if (buffer == NULL || delimiter == NULL) return SOCKET_UTIL_FAIL;

    size_t total_read = 0;
    ssize_t n;
    // size_t delim_len = strlen(delimiter);
    char temp_buf[TEMP_BUF_SIZE];

    // Loop through the recv of bytes from fd, and use partial buffer (temp_buf) to fill buffer
    while (total_read < max_len - 1) { // Leave space for null terminator
        size_t remaining = max_len - 1 - total_read;  // Remaining space in buffer.
        size_t to_read;
        if (remaining < TEMP_BUF_SIZE) {
            to_read = remaining;
        } else {
            to_read = TEMP_BUF_SIZE;
        }

        // Reads to_read number of bytes from the socket file descriptor 
        n = recv(fd, temp_buf, to_read, 0);
        if (n < 0) {
            fprintf(stderr, "Failed to recv from fd\n");
            return SOCKET_UTIL_FAIL;
        } else if (n == 0) {
            // connection closed
            break;
        }
        // copy into main buffer at correct position
        memcpy(buffer + total_read, temp_buf, n);
        // increment the total amount read.
        total_read += n;
        // Ensure safe string operations
        buffer[total_read] = '\0';  

        if (strstr(buffer, delimiter) != NULL) {
            return total_read;
        }
    }

    return SOCKET_UTIL_FAIL;  // Delimiter not found or buffer exhausted
}


/**
 * Streams exactly `length` bytes from `src_fd` to `dest_fd`,
 * while storing them into a malloced buffer.
 * 
 * On success:
 *   - returns SOCKET_UTIL_SUCCESS,
 *   - sets *`out_buf` to a malloc'ed buffer (must be freed by caller).
 * 
 * The caller already knows the length since they provided it as input.
 */
int stream_and_store(int src_fd, int dest_fd, ssize_t length, char **out_buf) {
    if (src_fd < 0 || dest_fd < 0 || length < 0 || !out_buf) {
        return SOCKET_UTIL_FAIL;
    }
    
    // Create the buffer for storing the out_buf. It will be returned.
    char *buffer = malloc(length);
    if (!buffer) {
        fprintf(stderr, "malloc error");
        return SOCKET_UTIL_FAIL;
    }
    
    // Partial recv maximum of TEMP_BUF_SIZE of bytes at a time 
    ssize_t total_read = 0;
    while (total_read < length) {
        ssize_t to_read = length - total_read;
        if (to_read > TEMP_BUF_SIZE) {
            to_read = TEMP_BUF_SIZE;
        }
        
        ssize_t n = recv(src_fd, buffer + total_read, to_read, 0);  // 0 flag for no special behaviour
        if (n <= 0) {
            fprintf(stderr, "cannot stream and store, recv error ");
            free(buffer);
            return SOCKET_UTIL_FAIL;
        } 
        
        if (send_all(dest_fd, buffer + total_read, n) == SOCKET_UTIL_FAIL) {
            fprintf(stderr, "cannot stream and store, Send error\n");
            free(buffer);
            return SOCKET_UTIL_FAIL;
        }
        
        total_read += n;
    }
    
    *out_buf = buffer;
    return SOCKET_UTIL_SUCCESS;
}


/**
 * Sets a timeout for read and write operations on a given socket file descriptor.
 *
 * Return:
 *  0 on success, -1 on failure.
 
int set_timeout(int fd, int seconds) {
    // TODO
    return -1;
}
*/
