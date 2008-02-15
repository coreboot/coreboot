#include <stdlib.h>

extern unsigned char _heap, _eheap;
static size_t free_mem_ptr = (size_t)&_heap;		/* Start of heap */
static size_t free_mem_end_ptr = (size_t)&_eheap;	/* End of heap */


void malloc_mark(malloc_mark_t *place)
{
	*place = free_mem_ptr;
}

void malloc_release(malloc_mark_t *ptr)
{
	free_mem_ptr = *ptr;
}

void *malloc(size_t size)
{
	void *p;

	if (size < 0)
		error("Error! malloc: Size < 0");
	if (free_mem_ptr <= 0)
		error("Error! malloc: Free_mem_ptr <= 0");

	free_mem_ptr = (free_mem_ptr + 3) & ~3;	/* Align */

	p = (void *) free_mem_ptr;
	free_mem_ptr += size;

	if (free_mem_ptr >= free_mem_end_ptr)
		error("Error! malloc: Free_mem_ptr >= free_mem_end_ptr");


	return p;
}

void free(void *where)
{
	/* Don't care */
}
