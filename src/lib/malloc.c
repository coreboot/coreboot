#include <stdlib.h>
#include <console/console.h>

#if CONFIG_DEBUG_MALLOC
#define MALLOCDBG(x...) printk(BIOS_SPEW, x)
#else
#define MALLOCDBG(x)
#endif

extern unsigned char _heap, _eheap;
static void *free_mem_ptr = &_heap;		/* Start of heap */
static void *free_mem_end_ptr = &_eheap;	/* End of heap */

void *malloc(size_t size)
{
	void *p;

	MALLOCDBG("%s Enter, size %ld, free_mem_ptr %p\n", __func__, size, free_mem_ptr);

	/* Checking arguments */
	if (size < 0)
		die("Error! malloc: size < 0");

	/* Overzealous linker check */
	if (free_mem_ptr <= 0)
		die("Error! malloc: Free_mem_ptr <= 0");

	free_mem_ptr = (void *)ALIGN((unsigned long)free_mem_ptr, 4);

	p = (void *) free_mem_ptr;
	free_mem_ptr += size;

	if (free_mem_ptr >= free_mem_end_ptr)
		die("Error! malloc: Out of memory (free_mem_ptr >= free_mem_end_ptr)");

	MALLOCDBG("malloc %p\n", p);

	return p;
}

void free(void *where)
{
	/* Don't care */
	MALLOCDBG("free %p\n", where);
}
