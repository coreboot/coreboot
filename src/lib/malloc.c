/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <stdlib.h>
#include <string.h>

#if CONFIG(DEBUG_MALLOC)
#define MALLOCDBG(x...) printk(BIOS_SPEW, x)
#else
#define MALLOCDBG(x...)
#endif

extern unsigned char _heap, _eheap;
static void *free_mem_ptr = &_heap;		/* Start of heap */
static void *free_mem_end_ptr = &_eheap;	/* End of heap */
static void *free_last_alloc_ptr = &_heap;	/* End of heap before
						   last allocation */

/* We don't restrict the boundary. This is firmware,
 * you are supposed to know what you are doing.
 */
void *memalign(size_t boundary, size_t size)
{
	void *p;

	MALLOCDBG("%s Enter, boundary %zu, size %zu, free_mem_ptr %p\n",
		__func__, boundary, size, free_mem_ptr);

	free_mem_ptr = (void *)ALIGN_UP((unsigned long)free_mem_ptr, boundary);

	p = free_mem_ptr;
	free_mem_ptr += size;
	/*
	 * Store last allocation pointer after ALIGN, as malloc() will
	 * return it. This may cause n bytes of gap between allocations
	 * where n < boundary.
	 */
	free_last_alloc_ptr = p;

	if (free_mem_ptr >= free_mem_end_ptr) {
		printk(BIOS_ERR, "memalign(boundary=%zu, size=%zu): failed: ",
				boundary, size);
		printk(BIOS_ERR, "Tried to round up free_mem_ptr %p to %p\n",
				p, free_mem_ptr);
		printk(BIOS_ERR, "but free_mem_end_ptr is %p\n",
				free_mem_end_ptr);
		die("Error! memalign: Out of memory (free_mem_ptr >= free_mem_end_ptr)");
	}

	MALLOCDBG("memalign %p\n", p);

	return p;
}

void *malloc(size_t size)
{
	return memalign(sizeof(u64), size);
}

void *calloc(size_t nitems, size_t size)
{
	void *p = malloc(nitems * size);
	if (p)
		memset(p, 0, nitems * size);

	return p;
}

void free(void *ptr)
{
	if (ptr == NULL)
		return;

	if (ptr < (void *)&_heap || ptr >= free_mem_end_ptr) {
		printk(BIOS_WARNING, "Pointer passed to %s is not "
					"pointing to the heap\n", __func__);
		return;
	}

	/*
	 * Rewind the heap pointer to the end of heap
	 * before the last successful malloc().
	 */
	if (ptr == free_last_alloc_ptr) {
		free_mem_ptr = free_last_alloc_ptr;
		free_last_alloc_ptr = NULL;
	}
}
