#include <stdlib.h>
#include <console/console.h>
#ifdef __SMM__
#include <cpu/x86/smm.h>
#endif

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

	MALLOCDBG("%s Enter, boundary %zu, size %zu, free_mem_ptr %p\n",
		__func__, boundary, size, free_mem_ptr);

	free_mem_ptr = (void *)ALIGN((unsigned long)free_mem_ptr, boundary);

	p = free_mem_ptr;
	free_mem_ptr += size;

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
#if CONFIG_SMM_TSEG && defined(__SMM__)
	if (!free_mem_ptr) {
		free_mem_ptr = &_heap + smi_get_tseg_base();
		free_mem_end_ptr = &_eheap + smi_get_tseg_base();
	}
#endif
	return memalign(sizeof(u64), size);
}
