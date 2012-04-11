#include <stdlib.h>
#include <console/console.h>

#if CONFIG_DEBUG_MALLOC
#define MALLOCDBG(x...) printk(BIOS_SPEW, x)
#else
#define MALLOCDBG(x...)
#endif

extern unsigned char _heap, _eheap;
static void *free_mem_ptr = &_heap;		/* Start of heap */
static void *free_mem_end_ptr = &_eheap;	/* End of heap */

/* We don't restrict the boundary. This is firmware,
 * you are supposed to know what you are doing.
 */
void *memalign(size_t boundary, size_t size)
{
	void *p;

	MALLOCDBG("%s Enter, boundary %ld, size %ld, free_mem_ptr %p\n",
		__func__, boundary, size, free_mem_ptr);

	/* Overzealous linker check */
	if (free_mem_ptr <= 0)
		die("Error! memalign: Free_mem_ptr <= 0");

	free_mem_ptr = (void *)ALIGN((unsigned long)free_mem_ptr, boundary);

	p = (void *) free_mem_ptr;
	free_mem_ptr += size;

	if (free_mem_ptr >= free_mem_end_ptr)
		die("Error! memalign: Out of memory (free_mem_ptr >= free_mem_end_ptr)");

	MALLOCDBG("memalign %p\n", p);

	return p;
}

void *malloc(size_t size)
{
	return memalign(sizeof(u64), size);
}

void free(void *where)
{
	/* Don't care */
	MALLOCDBG("free %p\n", where);
}
