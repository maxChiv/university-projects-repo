/*===========================================
main.c - COMP30023 Project 2
    -> Entry point of Web Proxy
Names: Max Chivers, Alif Farul Azim
Project: Web Proxy
===========================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "proxy.h"

int parse_arguments(int argc, char *argv[], int *port, int *enable_cache);
void handle_sigint(int sig);

int main(int argc, char *argv[]) {
    // REGISTER HANDLERS
    signal(SIGINT, handle_sigint);  // Handles CrtlC (SIGINT)
    signal(SIGSEGV, handle_sigint); // handle segfaults too
    atexit(proxy_cleanup);  // Handles cleanup of proxy at exit

    int port;
    int enable_cache = 0;

    if (parse_arguments(argc, argv, &port, &enable_cache) < 0) {
        fprintf(stderr, "Usage: ./htproxy -p <port> [-c]\n");
        return EXIT_FAILURE;
    }

    // Call the main proxy loop
    run_proxy(port, enable_cache);

    return 0;
}

int parse_arguments(int argc, char *argv[], int *port, int *enable_cache) {
    if (argc < 3 || argc > 4) {
        return -1;
    }

    if (strcmp(argv[1], "-p") != 0) {
        return -1;
    }

    *port = atoi(argv[2]);

    if (argc == 4) {
        if (strcmp(argv[3], "-c") != 0) {
            return -1;
        }
        *enable_cache = 1;
    }

    return 0;
}

/* HANDLE ON ABRUPT EXIT */
void handle_sigint(int sig) {
    fprintf(stderr, "Caught signal %d, cleaning up...\n", sig);
    proxy_cleanup(); // Clean proxy resources
    exit(EXIT_FAILURE);
}