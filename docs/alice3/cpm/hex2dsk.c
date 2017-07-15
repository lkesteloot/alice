
// Writes the CP/M loader, CP/M image, and BIOS hex files to a floppy disk
// image.  Optionally uses a template disk in order to retain existing files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../emulator/readhex.h"

#define SECTOR_SIZE 128     // In bytes.
#define TRACK_SIZE 64       // In sectors.

void usage() {
    fprintf(stderr, "Usage: hex2dsk --template template.dsk cpm_loader.hex cpm22.hex alice3_cbios.hex out.dsk\n");
    exit(-1);
}

// Returns the next param but does not advance.
char *peek_param(int argc, char *argv[]) {
    if (argc == 0) {
        usage();
    }

    return argv[0];
}

// Returns the next param and advances.
char *next_param(int *argc, char ***argv) {
    char *param = peek_param(*argc, *argv);

    (*argc)--;
    (*argv)++;

    return param;
}

// Caller must free buffer. Returns the number of bytes read in size_read.
unsigned char *read_hex_file(char *filename, int address, int max_size, int *size_read) {
    struct memory_desc md;
    int size = 1024*64;
    unsigned char *buffer = (unsigned char *) malloc(size);

    memset(buffer, '\0', size);
    memory_desc_init(&md, buffer, address, size);

    // Read the hex file into our buffer.
    FILE *f = fopen(filename, "ra");
    int success = read_hex(f, memory_desc_store, &md, 1);
    if (!success) {
        fprintf(stderr, "hex2dsk: Failed to read the input hex file \"%s\"\n", filename);
        exit(-1);
    }
    fclose(f);

    // Sanity check.
    if (md.size_written > max_size) {
        fprintf(stderr, "hex2dsk: Hex file \"%s\" is too large (%ld > %d).\n", filename, md.size_written, max_size);
        exit(-1);
    }

    *size_read = md.size_written;
    fprintf(stderr, "hex2dsk: Read %d bytes from \"%s\".\n", *size_read, filename);

    return buffer;
}

int main(int argc, char *argv[]) {
    // Parse command line.
    next_param(&argc, &argv);
    char *template = NULL;
    if (strcmp(peek_param(argc, argv), "--template") == 0) {
        next_param(&argc, &argv);
        template = next_param(&argc, &argv);
    }
    char *cpm_loader = next_param(&argc, &argv);
    char *cpm22 = next_param(&argc, &argv);
    char *bios = next_param(&argc, &argv);
    char *output = next_param(&argc, &argv);
    if (argc != 0) {
        usage();
    }

    // Make an 8 MB image.
    int size = 1024*1024*8;
    char *disk = (char *) malloc(size);

    // Fill with empty bytes.
    memset(disk, 0xE5, size);

    // Optionally overlay with template diskette.
    if (template != NULL) {
        FILE *f = fopen(template, "rb");
        fread(disk, 1, size, f);
        fclose(f);
    }

    unsigned char *buffer;
    int size_read;

    // Read CP/M loader. This is just two sectors that we load at 0x100.
    buffer = read_hex_file(cpm_loader, 0x100, 2*SECTOR_SIZE, &size_read);
    memmove(disk, buffer, SECTOR_SIZE*2);
    free(buffer);

    // Read CP/M image.
    buffer = read_hex_file(cpm22, 0xE400, 46*SECTOR_SIZE, &size_read);
    memmove(disk + SECTOR_SIZE*2, buffer, SECTOR_SIZE*46);
    free(buffer);

    // Read BIOS image.
    buffer = read_hex_file(bios, 0xFA00, SECTOR_SIZE*TRACK_SIZE, &size_read);
    memmove(disk + SECTOR_SIZE*TRACK_SIZE, buffer, size_read);
    free(buffer);

    // Write diskette.
    FILE *f = fopen(output, "wb");
    fwrite(disk, 1, size, f);
    fclose(f);

    return 0;
}

