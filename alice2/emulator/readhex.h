
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
int read_hex(FILE *f, unsigned char buf[], int max_bytes, int bad_checksum_is_error);

#ifdef __cplusplus
}
#endif

#endif // __readhex_h__
