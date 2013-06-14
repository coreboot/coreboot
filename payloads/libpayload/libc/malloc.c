/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008-2010 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * This is a classically weak malloc() implementation. We have a relatively
 * small and static heap, so we take the easy route with an O(N) loop
 * through the tree for every malloc() and free(). Obviously, this doesn't
 * scale past a few hundred KB (if that).
 *
 * We're also susceptible to the usual buffer overrun poisoning, though the
 * risk is within acceptable ranges for this implementation (don't overrun
 * your buffers, kids!).
 */

#define IN_MALLOC_C
#include <libpayload.h>

extern char _heap, _eheap;	/* Defined in the ldscript. */

static void *hstart = (void *)&_heap;
static void *hend = (void *)&_eheap;

typedef u64 hdrtype_t;
#define HDRSIZE (sizeof(hdrtype_t))

#define SIZE_BITS ((HDRSIZE << 3) - 7)
#define MAGIC     (((hdrtype_t)0x2a) << (SIZE_BITS + 1))
#define FLAG_FREE (((hdrtype_t)0x01) << (SIZE_BITS + 0))
#define MAX_SIZE  ((((hdrtype_t)0x01) << SIZE_BITS) - 1)

#define SIZE(_h) ((_h) & MAX_SIZE)

#define _HEADER(_s, _f) ((hdrtype_t) (MAGIC | (_f) | ((_s) & MAX_SIZE)))

#define FREE_BLOCK(_s) _HEADER(_s, FLAG_FREE)
#define USED_BLOCK(_s) _HEADER(_s, 0)

#define IS_FREE(_h) (((_h) & (MAGIC | FLAG_FREE)) == (MAGIC | FLAG_FREE))
#define HAS_MAGIC(_h) (((_h) & MAGIC) == MAGIC)

static int free_aligned(void* addr);
void print_malloc_map(void);

#ifdef CONFIG_DEBUG_MALLOC
static int heap_initialized = 0;
static int minimal_free = 0;
#endif

static void setup(hdrtype_t volatile *start, int size)
{
	*start = FREE_BLOCK(size);

#ifdef CONFIG_DEBUG_MALLOC
	heap_initialized = 1;
	minimal_free  = size;
#endif
}

static void *alloc(int len)
{
	hdrtype_t header;
	hdrtype_t volatile *ptr = (hdrtype_t volatile *) hstart;

	/* Align the size. */
	len = (len + HDRSIZE - 1) & ~(HDRSIZE - 1);

	if (!len || len > MAX_SIZE)
		return (void *)NULL;

	/* Make sure the region is setup correctly. */
	if (!HAS_MAGIC(*ptr))
		setup(ptr, (int)((&_eheap - &_heap) - HDRSIZE));

	/* Find some free space. */
	do {
		header = *ptr;
		int size = SIZE(header);

		if (!HAS_MAGIC(header) || size == 0) {
			printf("memory allocator panic. (%s%s)\n",
			       !HAS_MAGIC(header) ? " no magic " : "",
				   size == 0 ? " size=0 " : "");
			halt();
		}

		if (header & FLAG_FREE) {
			if (len <= size) {
				hdrtype_t volatile *nptr = (hdrtype_t volatile *)((int)ptr + HDRSIZE + len);
				int nsize = size - (HDRSIZE + len);

				/* If there is still room in this block,
				 * then mark it as such otherwise account
				 * the whole space for that block.
				 */

				if (nsize > 0) {
					/* Mark the block as used. */
					*ptr = USED_BLOCK(len);

					/* Create a new free block. */
					*nptr = FREE_BLOCK(nsize);
				} else {
					/* Mark the block as used. */
					*ptr = USED_BLOCK(size);
				}

				return (void *)((int)ptr + HDRSIZE);
			}
		}

		ptr = (hdrtype_t volatile *)((int)ptr + HDRSIZE + size);

	} while (ptr < (hdrtype_t *) hend);

	/* Nothing available. */
	return (void *)NULL;
}

static void _consolidate(void)
{
	void *ptr = hstart;

	while (ptr < hend) {
		void *nptr;
		hdrtype_t hdr = *((hdrtype_t *) ptr);
		unsigned int size = 0;

		if (!IS_FREE(hdr)) {
			ptr += HDRSIZE + SIZE(hdr);
			continue;
		}

		size = SIZE(hdr);
		nptr = ptr + HDRSIZE + SIZE(hdr);

		while (nptr < hend) {
			hdrtype_t nhdr = *((hdrtype_t *) nptr);

			if (!(IS_FREE(nhdr)))
				break;

			size += SIZE(nhdr) + HDRSIZE;

			*((hdrtype_t *) nptr) = 0;

			nptr += (HDRSIZE + SIZE(nhdr));
		}

		*((hdrtype_t *) ptr) = FREE_BLOCK(size);
		ptr = nptr;
	}
}

void free(void *ptr)
{
	hdrtype_t hdr;

	if (free_aligned(ptr)) return;

	ptr -= HDRSIZE;

	/* Sanity check. */
	if (ptr < hstart || ptr >= hend)
		return;

	hdr = *((hdrtype_t *) ptr);

	/* Not our header (we're probably poisoned). */
	if (!HAS_MAGIC(hdr))
		return;

	/* Double free. */
	if (hdr & FLAG_FREE)
		return;

	*((hdrtype_t *) ptr) = FREE_BLOCK(SIZE(hdr));
	_consolidate();
}

void *malloc(size_t size)
{
	return alloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
	size_t total = nmemb * size;
	void *ptr = alloc(total);

	if (ptr)
		memset(ptr, 0, total);

	return ptr;
}

void *realloc(void *ptr, size_t size)
{
	void *ret, *pptr;
	unsigned int osize;

	if (ptr == NULL)
		return alloc(size);

	pptr = ptr - HDRSIZE;

	if (!HAS_MAGIC(*((hdrtype_t *) pptr)))
		return NULL;

	/* Get the original size of the block. */
	osize = SIZE(*((hdrtype_t *) pptr));

	/*
	 * Free the memory to update the tables - this won't touch the actual
	 * memory, so we can still use it for the copy after we have
	 * reallocated the new space.
	 */
	free(ptr);
	ret = alloc(size);

	/*
	 * if ret == NULL, then doh - failure.
	 * if ret == ptr then woo-hoo! no copy needed.
	 */
	if (ret == NULL || ret == ptr)
		return ret;

	/* Copy the memory to the new location. */
	memcpy(ret, ptr, osize > size ? size : osize);

	return ret;
}

struct align_region_t
{
	int alignment;
	/* start in memory, and size in bytes */
	void* start;
	int size;
	/* layout within a region:
	  - num_elements bytes, 0: free, 1: used, 2: used, combines with next
	  - padding to alignment
	  - data section
	  - waste space

	  start_data points to the start of the data section
	*/
	void* start_data;
	/* number of free blocks sized "alignment" */
	int free;
	struct align_region_t *next;
};

static struct align_region_t* align_regions = 0;

static struct align_region_t *allocate_region(int alignment, int num_elements)
{
	struct align_region_t *new_region;
#ifdef CONFIG_DEBUG_MALLOC
	printf("%s(old align_regions=%p, alignment=%u, num_elements=%u)\n",
			__func__, align_regions, alignment, num_elements);
#endif

	new_region = malloc(sizeof(struct align_region_t));

	if (!new_region)
		return NULL;
	new_region->alignment = alignment;
	new_region->start = malloc((num_elements+1) * alignment + num_elements);
	if (!new_region->start) {
		free(new_region);
		return NULL;
	}
	new_region->start_data = (void*)((u32)(new_region->start + num_elements + alignment - 1) & (~(alignment-1)));
	new_region->size = num_elements * alignment;
	new_region->free = num_elements;
	new_region->next = align_regions;
	memset(new_region->start, 0, num_elements);
	align_regions = new_region;
	return new_region;
}


static int free_aligned(void* addr)
{
	struct align_region_t *reg = align_regions;
	while (reg != 0)
	{
		if ((addr >= reg->start_data) && (addr < reg->start_data + reg->size))
		{
			int i = (addr-reg->start_data)/reg->alignment;
			while (((u8*)reg->start)[i]==2)
			{
				((u8*)reg->start)[i++]=0;
				reg->free++;
			}
			((u8*)reg->start)[i]=0;
			reg->free++;
			return 1;
		}
		reg = reg->next;
	}
	return 0;
}

void *memalign(size_t align, size_t size)
{
	if (size == 0) return 0;
	if (align_regions == 0) {
		align_regions = malloc(sizeof(struct align_region_t));
		if (align_regions == NULL)
			return NULL;
		memset(align_regions, 0, sizeof(struct align_region_t));
	}
	struct align_region_t *reg = align_regions;
look_further:
	while (reg != 0)
	{
		if ((reg->alignment == align) && (reg->free >= (size + align - 1)/align))
		{
#ifdef CONFIG_DEBUG_MALLOC
			printf("  found memalign region. %x free, %x required\n", reg->free, (size + align - 1)/align);
#endif
			break;
		}
		reg = reg->next;
	}
	if (reg == 0)
	{
#ifdef CONFIG_DEBUG_MALLOC
		printf("  need to allocate a new memalign region\n");
#endif
		/* get align regions */
		reg = allocate_region(align, (size<1024)?(1024/align):(((size-1)/align)+1));
#ifdef CONFIG_DEBUG_MALLOC
		printf("  ... returned %p\n", align_regions);
#endif
	}
	if (reg == 0) {
		/* Nothing available. */
		return (void *)NULL;
	}

	int i, count = 0, target = (size+align-1)/align;
	for (i = 0; i < (reg->size/align); i++)
	{
		if (((u8*)reg->start)[i] == 0)
		{
			count++;
			if (count == target) {
				count = i+1-count;
				for (i=0; i<target-1; i++)
				{
					((u8*)reg->start)[count+i]=2;
				}
				((u8*)reg->start)[count+target-1]=1;
				reg->free -= target;
				return reg->start_data+(align*count);
			}
		} else {
			count = 0;
		}
	}
	/* The free space in this region is fragmented,
	   so we will move on and try the next one: */
	reg = reg->next;
	goto look_further; // end condition is once a new region is allocated - it always has enough space
}

/* This is for debugging purposes. */
#ifdef CONFIG_DEBUG_MALLOC
void print_malloc_map(void)
{
	void *ptr = hstart;
	int free_memory = 0;

	while (ptr < hend) {
		hdrtype_t hdr = *((hdrtype_t *) ptr);

		if (!HAS_MAGIC(hdr)) {
			if (heap_initialized)
				printf("Poisoned magic - we're toast\n");
			else
				printf("No magic yet - going to initialize\n");
			break;
		}

		/* FIXME: Verify the size of the block. */

		printf("%x: %s (%x bytes)\n",
		       (unsigned int)(ptr - hstart),
		       hdr & FLAG_FREE ? "FREE" : "USED", SIZE(hdr));

		if (hdr & FLAG_FREE)
			free_memory += SIZE(hdr);

		ptr += HDRSIZE + SIZE(hdr);
	}

	if (free_memory && (minimal_free > free_memory))
		minimal_free = free_memory;
	printf("Maximum memory consumption: %d bytes\n",
		(unsigned int)(&_eheap - &_heap) - HDRSIZE - minimal_free);
}
#endif
