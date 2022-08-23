/*
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
#include <stdint.h>

struct memory_type {
	void *start;
	void *end;
	struct align_region_t* align_regions;
#if CONFIG(LP_DEBUG_MALLOC)
	int magic_initialized;
	size_t minimal_free;
	const char *name;
#endif
};

extern char _heap, _eheap;	/* Defined in the ldscript. */

static struct memory_type default_type =
	{ (void *)&_heap, (void *)&_eheap, NULL
#if CONFIG(LP_DEBUG_MALLOC)
	, 0, 0, "HEAP"
#endif
	};
static struct memory_type *const heap = &default_type;
static struct memory_type *dma = &default_type;

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

static int free_aligned(void* addr, struct memory_type *type);
void print_malloc_map(void);

void init_dma_memory(void *start, u32 size)
{
	if (dma_initialized()) {
		printf("ERROR: %s called twice!\n", __func__);
		return;
	}

	/*
	 * DMA memory might not be zeroed by coreboot on stage loading, so make
	 * sure we clear the magic cookie from last boot.
	 */
	*(hdrtype_t *)start = 0;

	dma = malloc(sizeof(*dma));
	dma->start = start;
	dma->end = start + size;
	dma->align_regions = NULL;

#if CONFIG(LP_DEBUG_MALLOC)
	dma->minimal_free = 0;
	dma->magic_initialized = 0;
	dma->name = "DMA";

	printf("Initialized cache-coherent DMA memory at [%p:%p]\n", start, start + size);
#endif
}

int dma_initialized()
{
	return dma != heap;
}

/* For boards that don't initialize DMA we assume all locations are coherent */
int dma_coherent(const void *ptr)
{
	return !dma_initialized() || (dma->start <= ptr && dma->end > ptr);
}

/* Find free block of size >= len */
static hdrtype_t volatile *find_free_block(int len, struct memory_type *type)
{
	hdrtype_t header;
	hdrtype_t volatile *ptr = (hdrtype_t volatile *)type->start;

	/* Align the size. */
	len = ALIGN_UP(len, HDRSIZE);

	if (!len || len > MAX_SIZE)
		return (void *)NULL;

	/* Make sure the region is setup correctly. */
	if (!HAS_MAGIC(*ptr)) {
		size_t size = (type->end - type->start) - HDRSIZE;
		*ptr = FREE_BLOCK(size);
#if CONFIG(LP_DEBUG_MALLOC)
		type->magic_initialized = 1;
		type->minimal_free = size;
#endif
	}

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

		if ((header & FLAG_FREE) && len <= size)
			return ptr;

		ptr = (hdrtype_t volatile *)((uintptr_t)ptr + HDRSIZE + size);

	} while (ptr < (hdrtype_t *) type->end);

	/* Nothing available. */
	return NULL;
}

/* Mark the block with length 'len' as used */
static void use_block(hdrtype_t volatile *ptr, int len)
{
	/* Align the size. */
	len = ALIGN_UP(len, HDRSIZE);

	hdrtype_t volatile *nptr = (hdrtype_t volatile *)
		((uintptr_t)ptr + HDRSIZE + len);
	int size = SIZE(*ptr);
	int nsize = size - (HDRSIZE + len);

	/*
	 * If there is still room in this block, then mark it as such otherwise
	 * account the whole space for that block.
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
}

static void *alloc(int len, struct memory_type *type)
{
	hdrtype_t volatile *ptr = find_free_block(len, type);

	if (ptr == NULL)
		return NULL;

	use_block(ptr, len);
	return (void *)((uintptr_t)ptr + HDRSIZE);
}

static void _consolidate(struct memory_type *type)
{
	void *ptr = type->start;

	while (ptr < type->end) {
		void *nptr;
		hdrtype_t hdr = *((hdrtype_t *) ptr);
		unsigned int size = 0;

		if (!IS_FREE(hdr)) {
			ptr += HDRSIZE + SIZE(hdr);
			continue;
		}

		size = SIZE(hdr);
		nptr = ptr + HDRSIZE + SIZE(hdr);

		while (nptr < type->end) {
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
	struct memory_type *type = heap;

	/* No action occurs on NULL. */
	if (ptr == NULL)
		return;

	/* Sanity check. */
	if (ptr < type->start || ptr >= type->end) {
		type = dma;
		if (ptr < type->start || ptr >= type->end)
			return;
	}

	if (free_aligned(ptr, type)) return;

	ptr -= HDRSIZE;
	hdr = *((hdrtype_t *) ptr);

	/* Not our header (we're probably poisoned). */
	if (!HAS_MAGIC(hdr))
		return;

	/* Double free. */
	if (hdr & FLAG_FREE)
		return;

	*((hdrtype_t *) ptr) = FREE_BLOCK(SIZE(hdr));
	_consolidate(type);
}

void *malloc(size_t size)
{
	return alloc(size, heap);
}

void *dma_malloc(size_t size)
{
	return alloc(size, dma);
}

void *calloc(size_t nmemb, size_t size)
{
	size_t total = nmemb * size;
	void *ptr = alloc(total, heap);

	if (ptr)
		memset(ptr, 0, total);

	return ptr;
}

void *realloc(void *ptr, size_t size)
{
	void *ret, *pptr;
	hdrtype_t volatile *block;
	unsigned int osize;
	struct memory_type *type = heap;

	if (ptr == NULL)
		return alloc(size, type);

	pptr = ptr - HDRSIZE;

	if (!HAS_MAGIC(*((hdrtype_t *) pptr)))
		return NULL;

	if (ptr < type->start || ptr >= type->end)
		type = dma;

	/* Get the original size of the block. */
	osize = SIZE(*((hdrtype_t *) pptr));

	/*
	 * Free the memory to update the tables - this won't touch the actual
	 * memory, so we can still use it for the copy after we have
	 * reallocated the new space.
	 */
	free(ptr);

	block = find_free_block(size, type);
	if (block == NULL)
		return NULL;

	ret = (void *)((uintptr_t)block + HDRSIZE);

	/*
	 * If ret == ptr, then no copy is needed. Otherwise, move the memory to
	 * the new location, which might be before the old one and overlap since
	 * the free() above includes a _consolidate().
	 */
	if (ret != ptr)
		memmove(ret, ptr, osize > size ? size : osize);

	/* Mark the block as used. */
	use_block(block, size);

	return ret;
}

struct align_region_t
{
	/* If alignment is 0 then the region represents a large region which
	 * has no metadata for tracking subelements. */
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

static inline int region_is_large(const struct align_region_t *r)
{
	return r->alignment == 0;
}

static inline int addr_in_region(const struct align_region_t *r, void *addr)
{
	return ((addr >= r->start_data) && (addr < r->start_data + r->size));
}

/* num_elements == 0 indicates a large aligned region instead of a smaller
 * region comprised of alignment-sized chunks. */
static struct align_region_t *allocate_region(int alignment, int num_elements,
					size_t size, struct memory_type *type)
{
	struct align_region_t *r;
	size_t extra_space;

#if CONFIG(LP_DEBUG_MALLOC)
	printf("%s(old align_regions=%p, alignment=%u, num_elements=%u, size=%zu)\n",
		__func__, type->align_regions, alignment, num_elements, size);
#endif

	r = malloc(sizeof(*r));

	if (r == NULL)
		return NULL;

	memset(r, 0, sizeof(*r));

	if (num_elements != 0) {
		r->alignment = alignment;
		r->size = num_elements * alignment;
		r->free = num_elements;
		/* Allocate enough memory for alignment requirements and
		 * metadata for each chunk. */
		extra_space = num_elements;
	} else {
		/* Large aligned allocation. Set alignment = 0. */
		r->alignment = 0;
		r->size = size;
		extra_space = 0;
	}

	r->start = alloc(r->size + alignment + extra_space, type);

	if (r->start == NULL) {
		free(r);
		return NULL;
	}

	r->start_data = (void *)ALIGN_UP((uintptr_t)r->start + extra_space,
					alignment);

	/* Clear any (if requested) metadata. */
	memset(r->start, 0, extra_space);

	/* Link the region with the rest. */
	r->next = type->align_regions;
	type->align_regions = r;

	return r;
}

static void try_free_region(struct align_region_t **prev_link)
{
	struct align_region_t *r = *prev_link;

	/* All large regions are immediately free-able. Non-large regions
	 * need to be checked for the fully freed state. */
	if (!region_is_large(r)) {
		if (r->free != r->size / r->alignment)
			return;
	}

	/* Unlink region from link list. */
	*prev_link = r->next;

	/* Free the data and metadata. */
	free(r->start);
	free(r);
}

static int free_aligned(void* addr, struct memory_type *type)
{
	struct align_region_t **prev_link = &type->align_regions;

	while (*prev_link != NULL)
	{
		if (!addr_in_region(*prev_link, addr)) {
			prev_link = &((*prev_link)->next);
			continue;
		}

		if (region_is_large(*prev_link)) {
			try_free_region(prev_link);
			return 1;
		}

		int i = (addr-(*prev_link)->start_data)/(*prev_link)->alignment;
		u8 *meta = (*prev_link)->start;
		while (meta[i] == 2)
		{
			meta[i++] = 0;
			(*prev_link)->free++;
		}
		meta[i] = 0;
		(*prev_link)->free++;
		try_free_region(prev_link);
		return 1;
	}
	return 0;
}

static void *alloc_aligned(size_t align, size_t size, struct memory_type *type)
{
	/* Define a large request to be 1024 bytes for either alignment or
	 * size of allocation. */
	const size_t large_request = 1024;

	if (size == 0) return 0;
	if (type->align_regions == 0) {
		type->align_regions = malloc(sizeof(struct align_region_t));
		if (type->align_regions == NULL)
			return NULL;
		memset(type->align_regions, 0, sizeof(struct align_region_t));
	}
	struct align_region_t *reg = type->align_regions;

	if (size >= large_request || align >= large_request) {
		reg = allocate_region(align, 0, size, type);
		if (reg == NULL)
			return NULL;
		return reg->start_data;
	}

look_further:
	while (reg != 0)
	{
		if ((reg->alignment == align) && (reg->free >= (size + align - 1)/align))
		{
#if CONFIG(LP_DEBUG_MALLOC)
			printf("  found memalign region. %u free, %zu required\n", reg->free, (size + align - 1)/align);
#endif
			break;
		}
		reg = reg->next;
	}
	if (reg == 0)
	{
#if CONFIG(LP_DEBUG_MALLOC)
		printf("  need to allocate a new memalign region\n");
#endif
		/* get align regions */
		reg = allocate_region(align, large_request/align, size, type);
#if CONFIG(LP_DEBUG_MALLOC)
		printf("  ... returned %p\n", reg);
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

void *memalign(size_t align, size_t size)
{
	return alloc_aligned(align, size, heap);
}

void *dma_memalign(size_t align, size_t size)
{
	return alloc_aligned(align, size, dma);
}

/* This is for debugging purposes. */
#if CONFIG(LP_DEBUG_MALLOC)
void print_malloc_map(void)
{
	struct memory_type *type = heap;
	void *ptr;
	int free_memory;

again:
	ptr = type->start;
	free_memory = 0;

	while (ptr < type->end) {
		hdrtype_t hdr = *((hdrtype_t *) ptr);

		if (!HAS_MAGIC(hdr)) {
			if (type->magic_initialized)
				printf("%s: Poisoned magic - we're toast\n", type->name);
			else
				printf("%s: No magic yet - going to initialize\n", type->name);
			break;
		}

		/* FIXME: Verify the size of the block. */

		printf("%s %x: %s (%llx bytes)\n", type->name,
		       (unsigned int)(ptr - type->start),
		       hdr & FLAG_FREE ? "FREE" : "USED", SIZE(hdr));

		if (hdr & FLAG_FREE)
			free_memory += SIZE(hdr);

		ptr += HDRSIZE + SIZE(hdr);
	}

	if (free_memory && (type->minimal_free > free_memory))
		type->minimal_free = free_memory;
	printf("%s: Maximum memory consumption: %zu bytes\n", type->name,
		(type->end - type->start) - HDRSIZE - type->minimal_free);

	if (type != dma) {
		type = dma;
		goto again;
	}
}
#endif
