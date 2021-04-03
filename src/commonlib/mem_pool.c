/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <commonlib/mem_pool.h>

void *mem_pool_alloc(struct mem_pool *mp, size_t sz)
{
	void *p;

	/* Make all allocations be at least 8 byte aligned. */
	sz = ALIGN_UP(sz, 8);

	/* Determine if any space available. */
	if ((mp->size - mp->free_offset) < sz)
		return NULL;

	p = &mp->buf[mp->free_offset];

	mp->free_offset += sz;
	mp->second_to_last_alloc = mp->last_alloc;
	mp->last_alloc = p;

	return p;
}

void mem_pool_free(struct mem_pool *mp, void *p)
{
	/* Determine if p was the most recent allocation. */
	if (p == NULL || mp->last_alloc != p)
		return;

	mp->free_offset = mp->last_alloc - mp->buf;
	mp->last_alloc = mp->second_to_last_alloc;
	/* No way to track allocation before this one. */
	mp->second_to_last_alloc = NULL;
}
