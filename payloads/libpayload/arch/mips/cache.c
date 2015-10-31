/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#include <arch/cache.h>


void dcache_clean_all(void)
{
	/* TODO */
}

void dcache_invalidate_all(void)
{
	/* TODO */
}
void dcache_clean_invalidate_all(void)
{
	/* TODO */
}

void tlb_invalidate_all(void)
{
	/* TODO */
}

unsigned int dcache_line_bytes(void)
{
	/* TO DO */
	return 0;
}

void dcache_mmu_disable(void)
{
	/* TODO */
}

void dcache_mmu_enable(void)
{
	/* TODO */
}

void cache_sync_instructions(void)
{
	/* TODO */
}

void mmu_init(void)
{
	/* TODO */
}

void mmu_disable_range(unsigned long start_mb, unsigned long size_mb)
{
	/* TODO */
}
void mmu_config_range(unsigned long start_mb, unsigned long size_mb,
						enum dcache_policy policy)
{
	/* TODO */
}
