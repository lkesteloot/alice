#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readhex.h"

typedef struct memory_addresses
{
    int *addresses;
    int count;
} memory_addresses;

void memory_addresses_store(void *arg, int address, unsigned char c)
{
    memory_addresses *mi = (struct memory_addresses *)arg;

    mi->addresses = (int*)realloc(mi->addresses, sizeof(int) * (mi->count + 1));
    mi->addresses[mi->count++] = address;
}

int compare(const void *p1, const void *p2)
{
    const int *i1 = (const int*)p1;
    const int *i2 = (const int*)p2;
    if(*i1 < *i2)
        return -1;
    if(*i1 == *i2)
        return 0;
    return 1;
}

// Convert a .hex file to a .bin file.
int main(int argc, char *argv[])
{
    char **argvp = argv + 1;
    int argcp = argc - 1;

    if (argc < 2) {
        fprintf(stderr, "usage: %s in.hex\n", argv[0]);
        return 1;
    }
    
    char *in_filename = argvp[0];

    FILE *inf = fopen(in_filename, "ra");
    if (inf == NULL) {
        perror(in_filename);
        return 1;
    }

    memory_addresses addresses;
    addresses.addresses = NULL;
    addresses.count = 0;
    int success = read_hex(inf, memory_addresses_store, &addresses, 1);
    fclose(inf);

    if (!success) {
        // The read_hex() routine will print the error.
        return 1;
    }

    qsort(addresses.addresses, addresses.count, sizeof(int), compare);
    int start = addresses.addresses[0];
    int length = 1;
    for(int i = 1; i < addresses.count; i++) {
        int a = addresses.addresses[i];
        if(a == start + length)
            length++;
        else {
            printf("0x%08X for 0x%08x (%d) bytes\n", start, length, length);
            start = a;
            length = 1;
        }
    }
    printf("0x%08X for 0x%08x (%d) bytes\n", start, length, length);

    return 0;
}
