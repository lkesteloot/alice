
#include "mem.h"

#define DEBUG 1

/*
    Each chunk has two three words at the beginning and one
    word at the end for overhead.  The three words at the beginning
    are the tag word, the size word, and the next-chunk pointer.
    The word at the end is the tag bit.

	Chunk:	Start tag word (0 = free, 1 = taken)
		Size of chunk, including overhead
		Next free chunk pointer (only if tag = 0)
		...
		Size of chunk (only if tag = 0)
		End tag word (0 = free, 1 = taken)

    Tag words are 0 if the chunk is free, 1 if the chunk is used.

    Size word is the size of the entire chunk, including overhead,
    in bytes.

    Next-chunk pointer points to the next chunk in the free list.
    It's only there if the chunk is free.  Otherwise that word
    is available to the application.

    Tag words are useful for merging chunks together when they're
    freed.  The newly free chunk checks the tag words on both sides.
    If either/both are free, the chunks are merged.

    The arena is an area of memory we malloc from.  The first and
    last words are "1" to look like the end and start tags of
    used chunks.
*/

#define INT(p, i)	(((int *)(p))[i])

#define TAG(p)		INT(p, 0)
#define SIZE(p)		INT(p, 1)
#define NEXT(p)		((void *)INT(p, 2))


static void *free_list;
static char arena[48*1024]; /* 48k of RAM */


void *
MEM_alloc(int size)
{
    static int was_init = 0;
    void *p, *prev_p, *next_p;
    int round, p_size;

    if (size == 0) {
	return NULL;
    }

    if (!was_init) {
	was_init = 1;

	p = arena;

	INT(arena, 0) = 1;  /* 

	free_list = arena;
	SIZE(free_list) = sizeof(arena);
	NEXT(free_list) = NULL;
    }

    /* need space for size */
    size += sizeof(int);

    /* round up to next int */
    round = sizeof(int) - 1;
    size = (size + round) & ~round;

    prev_p = NULL;

    for (p = free_list; p != NULL; p = NEXT(p)) {
	p_size = SIZE(p);

	if (p_size >= size) {
	    if (p_size - size > sizeof(int) + sizeof(void *)) {
		/* make a new block with the remaining space */

		next_p = (char *)p + size;

		if (prev_p == NULL) {
		    free_list = next_p;
		} else {
		    NEXT(prev_p) = next_p;
		}

		SIZE(next_p) = p_size - size;
		NEXT(next_p) = NEXT(p);

		SIZE(p) = size;
	    } else {
		/* remove it from the free list */

		if (prev_p == NULL) {
		    free_list = NEXT(p);
		} else {
		    NEXT(prev_p) = NEXT(p);
		}
	    }

#if DEBUG
	    printf("mem: allocated %d bytes from %d\n",
		size, (char *)p - arena);
#endif

	    return &NEXT(p);
	}

	prev_p = p;
    }

    /* out of memory */
    return NULL;
}


void
MEM_free(void *p)
{
    p = (int *)p - 1;

#if DEBUG
    printf("mem: freeing %d bytes from %d\n", SIZE(p), (char *)p - arena);
#endif

    NEXT(p) = free_list;

    free_list = p;
}
