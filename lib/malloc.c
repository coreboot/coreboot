/*
 * malloc -- simple non-freeing malloc. 
 * 
 *
 * Author unkown. I guess we need a 'tomb of the unknown coder'. 
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 *
 */
#include <stdlib.h>
#include <console/console.h>

#if 0
#define MALLOCDBG(x...)
#else
#define MALLOCDBG(x...) printk (BIOS_SPEW, x)
#endif

/* instead of ldscript magic, just declare an array. The array 
 * will consume no bytes in the image. And it won't run into trouble
 * the way the V2 allocator could. 
 */

#define HEAPSIZE (256*1024)
static unsigned char heap[HEAPSIZE];
static size_t free_mem_ptr = (size_t)&heap;		/* Start of heap */
static size_t free_mem_end_ptr = (size_t)&heap[HEAPSIZE];
/* to keep gcc etc. happy ... */
typedef size_t malloc_mark_t;

void malloc_mark(malloc_mark_t *place)
{
	*place = free_mem_ptr;
	printk(BIOS_SPEW, "malloc_mark 0x%08lx\n", (unsigned long)free_mem_ptr);
}

void malloc_release(malloc_mark_t *ptr)
{
	free_mem_ptr = *ptr;
	printk(BIOS_SPEW, "malloc_release 0x%08lx\n", (unsigned long)free_mem_ptr);
}

void *malloc(size_t size)
{
	void *p;

	MALLOCDBG("%s Enter, size %d, free_mem_ptr %p\n", __FUNCTION__, size, free_mem_ptr);
	if (size < 0)
		die("Error! malloc: Size < 0");
	if (free_mem_ptr <= 0)
		die("Error! malloc: Free_mem_ptr <= 0");

	free_mem_ptr = (free_mem_ptr + 3) & ~3;	/* Align */

	p = (void *) free_mem_ptr;
	free_mem_ptr += size;

	if (free_mem_ptr >= free_mem_end_ptr)
		die("Error! malloc: free_mem_ptr >= free_mem_end_ptr");

	MALLOCDBG("malloc 0x%08lx\n", (unsigned long)p);

	return p;
}

void free(void *where)
{
	/* Don't care */
}
