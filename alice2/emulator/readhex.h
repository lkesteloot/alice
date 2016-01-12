
#ifndef __readhex_h__
#define __readhex_h__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Parses an Intel HEX file.
//
// https://en.wikipedia.org/wiki/Intel_HEX
//
// Returns 1 on success, 0 on failure (file was longer than max_bytes or if
// there was a parsing error). Does not close the file.
//
// If bad_checksum_is_error is non-zero (true) and a record checksum does not
// match the function's calculation of the checksum, return failure.
//
typedef void (*memory_func)(void *arg, int address, unsigned char c);
int read_hex(FILE *f, memory_func memory, void *memory_arg, int bad_checksum_is_error);

//
// Convenience struct and functions
//
// Usage:
//
//    struct memory_desc md;
//    memory_desc_init(&md, buffer, offset_address, buffer_size);
//    int success_if_1 = read_hex(file, memory_desc_store, &md, bad_checksum_is_error);
//
// Example:
//
//    unsigned char buffer[256];
//    memory_desc_init(&md, buffer, 0x300000, sizeof(buffer));
//    // Will store any hexfile bytes between 0x300000 and 0x3000ff into buffer:
//    int success_if_1 = read_hex(file, memory_desc_store, &md, 0);
//

struct memory_desc 
{
    unsigned char *p;
    off_t offset;
    size_t size;

    // This is filled in by memory_desc_store to note the largest size written by
    // the hex file:
    size_t size_written;
};

void memory_desc_init(struct memory_desc *mi, unsigned char *p, off_t offset, size_t size);
void memory_desc_store(void *arg, int address, unsigned char c);

#ifdef __cplusplus
}
#endif

#endif // __readhex_h__
