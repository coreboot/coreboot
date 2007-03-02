/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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
 */

/*
 * Simple non-freeing malloc. There have been about a million versions of
 * this but we need one with a known author. Almost every OS and bootloader
 * has had this at some time or other. 
 */

#include <stdlib.h>
#include <console/console.h>

#if 0
#define MALLOCDBG(x...)
#else
#define MALLOCDBG(x...) printk (BIOS_SPEW, x)
#endif

/* instead of ldscript magic, just declare an array. The array 
 * will consume no bytes in the image, and it won't run into trouble
 * the way the V2 allocator could. We do not provide zero'd memory. 
 * note that the execute-in-place code in top of flash is not allowed to call malloc, 
 * since we can't link this in to it. The FLASH-based code should always be dead simple. 
 * (in fact, it's not clear we need malloc at all any more -- we're doing our best to 
 * remove all usage of it -- the only real users were elfboot and lzma, and we have
 * removed its usage in elfboot, and will try to remove its usage in lzma). 
 */

#define HEAPSIZE (256*1024)
static unsigned char heap[HEAPSIZE];
static unsigned char *free_mem_ptr = heap;
static unsigned long freebytes = HEAPSIZE;

void *malloc(size_t size)
{
	void *p;

	MALLOCDBG("%s Enter, size %d, free_mem_ptr %p\n", __FUNCTION__, size,
		  free_mem_ptr);

	if (size > freebytes) {
		printk(BIOS_ERR, "OUT OF MEMORY for alloc of %d bytes\n", size);
		die("OUT OF MEMORY\n");
	}

	size = (size + 3) & 3;	/* Align */

	p = free_mem_ptr;
	free_mem_ptr += size;
	freebytes -= size;

	MALLOCDBG("malloc 0x%08lx\n", (unsigned long)p);

	return p;
}

void free(void *where)
{
	/* Don't care */
}
