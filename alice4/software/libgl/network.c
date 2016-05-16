
// Implementation of connection.h interface.

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "connection.h"

#define BUFFER_SIZE 1024

static int socket_fd = -1;
static int trace_network = 0;
static unsigned char buffer[BUFFER_SIZE];
static int buffer_length = 0;

// For connection.h.
void open_connection() {
    struct sockaddr_in sa;
    int res;

    socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&sa, 0, sizeof sa);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(25423);
    res = inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr);

    if (connect(socket_fd, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
}

// For connection.h.
void send_byte(unsigned char b) {
    if (socket_fd == -1) {
        fprintf(stderr, "Connection is not open.\n");
        exit(EXIT_FAILURE);
    }

    if (trace_network) {
        printf("Sending byte: 0x%02x\n", (int)b);
    }

    if (buffer_length == BUFFER_SIZE) {
        flush();
    }
    buffer[buffer_length++] = b;
}

unsigned char receive_byte() {
    if (socket_fd == -1) {
        fprintf(stderr, "Connection is not open.\n");
        exit(EXIT_FAILURE);
    }

    flush();

    unsigned char b;
    while (1) {
        int len = read(socket_fd, &b, 1);
        if (len == -1) {
            socket_fd = -1;
            perror("reading socket");
            exit(0);
        }

        if (len == 0) {
            socket_fd = -1;
            printf("Server closed connection.\n");
            exit(0);
        }

        if (len == 1) {
            if (trace_network) {
                printf("Received byte: 0x%02x\n", (int)b);
            }
            return b;
        }
    }
}

// For connection.h.
void flush() {
    if (buffer_length > 0) {
        write(socket_fd, buffer, buffer_length);
        buffer_length = 0;
    }
}

// For connection.h.
void close_connection() {
    flush();
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}
