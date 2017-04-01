
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
static uint8_t buffer[BUFFER_SIZE];
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
    if(getenv("PORT"))
        sa.sin_port = htons(atoi(getenv("PORT")));
    else
        sa.sin_port = htons(25423);
    if(getenv("ADDR"))
        res = inet_pton(AF_INET, getenv("ADDR"), &sa.sin_addr);
    else
        res = inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr);

    if (connect(socket_fd, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

}

// For connection.h.
void send_uint8(uint8_t b) {
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

static FILE *capture_file = NULL;

void open_capture() {
    if(getenv("CAPTURE_FILE") != NULL) {
        capture_file = fopen(getenv("CAPTURE_FILE"), "wb");
    }
}

void send_and_capture_uint8(uint8_t b) {
    send_uint8(b);
    if(capture_file != NULL)
        fwrite(&b, 1, 1, capture_file);
}

uint8_t receive_uint8() {
    if (socket_fd == -1) {
        fprintf(stderr, "Connection is not open.\n");
        exit(EXIT_FAILURE);
    }

    flush();

    uint8_t b;
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

// Send a string as a length and the bytes. Max string length is 255.
void send_and_capture_string(char *s) {
    send_and_capture_uint8(strlen(s));
    while (*s != '\0') {
        send_and_capture_uint8(*s);
        s++;
    }
}

// Little-endian.
void send_and_capture_uint16(uint16_t x) {
    if (trace_network) {
        printf("Sending short %d\n", x);
    }
    send_and_capture_uint8(x & 0xFF);
    send_and_capture_uint8(x >> 8);
}

// Little-endian.
void send_and_capture_uint32(uint32_t x) {
    if (trace_network) {
        printf("Sending int32_t %u\n", x);
    }
    send_and_capture_uint8(x & 0xFF);
    send_and_capture_uint8((x >> 8) & 0xFF);
    send_and_capture_uint8((x >> 16) & 0xFF);
    send_and_capture_uint8((x >> 24) & 0xFF);
}

// Little-endian.
void send_uint16(uint16_t x) {
    if (trace_network) {
        printf("Sending short %d\n", x);
    }
    send_uint8(x & 0xFF);
    send_uint8(x >> 8);
}

// Little-endian.
void send_uint32(uint32_t x) {
    if (trace_network) {
        printf("Sending int32_t %u\n", x);
    }
    send_uint8(x & 0xFF);
    send_uint8((x >> 8) & 0xFF);
    send_uint8((x >> 16) & 0xFF);
    send_uint8((x >> 24) & 0xFF);
}

// Little-endian.
uint16_t receive_uint16() {
    unsigned char low = receive_uint8();
    unsigned char high = receive_uint8();

    return high*256 + low;
}

// Little-endian.
uint32_t receive_uint32() {
    int32_t value = 0;

    value |= receive_uint8() << 0;
    value |= receive_uint8() << 8;
    value |= receive_uint8() << 16;
    value |= receive_uint8() << 24;

    return value;
}

