/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
	mp->last_alloc = p;

	return p;
}

void mem_pool_free(struct mem_pool *mp, void *p)
{
	/* Determine if p was the most recent allocation. */
	if (p == NULL || mp->last_alloc != p)
		return;

	mp->free_offset = mp->last_alloc - mp->buf;
	/* No way to track allocation before this one. */
	mp->last_alloc = NULL;
}
