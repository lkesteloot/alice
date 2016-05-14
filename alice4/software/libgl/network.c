
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

static int socket_fd = -1;

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

void send_byte(unsigned char b) {
    if (socket_fd == -1) {
        fprintf(stderr, "Connection is not open.\n");
        exit(EXIT_FAILURE);
    }

    printf("Sending byte: 0x%02x\n", (int)b);
    write(socket_fd, &b, 1);
}

void close_connection() {
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}
