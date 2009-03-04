#include <stdlib.h>
#include <console/console.h>

#if 0
#define MALLOCDBG(x)
#else
#define MALLOCDBG(x) printk_spew x
#endif
extern unsigned char _heap, _eheap;
static size_t free_mem_ptr = (size_t)&_heap;		/* Start of heap */
static size_t free_mem_end_ptr = (size_t)&_eheap;	/* End of heap */


void malloc_mark(malloc_mark_t *place)
{
	*place = free_mem_ptr;
	printk_spew("malloc_mark 0x%08lx\n", (unsigned long)free_mem_ptr);
}

void malloc_release(malloc_mark_t *ptr)
{
	free_mem_ptr = *ptr;
	printk_spew("malloc_release 0x%08lx\n", (unsigned long)free_mem_ptr);
}

void *malloc(size_t size)
{
	void *p;

	MALLOCDBG(("%s Enter, size %ld, free_mem_ptr 0x%08lx\n", __func__, size, free_mem_ptr));
	if (size < 0)
		die("Error! malloc: Size < 0");
	if (free_mem_ptr <= 0)
		die("Error! malloc: Free_mem_ptr <= 0");

	free_mem_ptr = (free_mem_ptr + 3) & ~3;	/* Align */

	p = (void *) free_mem_ptr;
	free_mem_ptr += size;

	if (free_mem_ptr >= free_mem_end_ptr)
		die("Error! malloc: free_mem_ptr >= free_mem_end_ptr");

	MALLOCDBG(("malloc 0x%08lx\n", (unsigned long)p));

	return p;
}

void free(void *where)
{
	/* Don't care */
}
