
// 32k heap


void printk(const char *fmt, ...);
#define error printk

static unsigned long free_mem_ptr = 0x20000;		/* Start of heap */
static unsigned long free_mem_end_ptr = 0x28000;	/* End of heap */


void *malloc(unsigned int size)
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
