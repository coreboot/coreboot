/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MEM_POOL_H_
#define _MEM_POOL_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/*
 * The memory pool allows one to allocate memory from a fixed size buffer that
 * also allows freeing semantics for reuse. However, the current limitation is
 * that only the two most recent allocations  can be freed (in exact reverse
 * order). If one tries to free any allocation that isn't at the top of the
 * allocation stack, or one allocates more than two buffers in a row without
 * freeing, it will result in a leak within the memory pool. (Two allocations
 * were chosen to optimize for the CBFS cache case which may need two buffers
 * to map a single compressed file, and will free them in reverse order.)
 *
 * You must ensure the backing buffer is 'alignment' aligned.
 */

struct mem_pool {
	uint8_t *buf;
	size_t size;
	size_t alignment;
	uint8_t *last_alloc;
	uint8_t *second_to_last_alloc;
	size_t free_offset;
};

#define MEM_POOL_INIT(buf_, size_, alignment_)	\
	{					\
		.buf = (buf_),			\
		.size = (size_),		\
		.alignment = (alignment_),	\
		.last_alloc = NULL,		\
		.second_to_last_alloc = NULL,	\
		.free_offset = 0,		\
	}

static inline void mem_pool_reset(struct mem_pool *mp)
{
	mp->last_alloc = NULL;
	mp->second_to_last_alloc = NULL;
	mp->free_offset = 0;
}

/* Initialize a memory pool. */
static inline void mem_pool_init(struct mem_pool *mp, void *buf, size_t sz,
					 size_t alignment)
{
	assert(alignment);
	assert((uintptr_t)buf % alignment == 0);

	mp->buf = buf;
	mp->size = sz;
	mp->alignment = alignment;
	mem_pool_reset(mp);
}

/* Allocate requested size from the memory pool. NULL returned on error. */
void *mem_pool_alloc(struct mem_pool *mp, size_t sz);

/* Free allocation from memory pool. */
void mem_pool_free(struct mem_pool *mp, void *alloc);

#endif /* _MEM_POOL_H_ */
