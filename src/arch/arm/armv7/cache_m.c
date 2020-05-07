/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * cache.c: Cache maintenance routines for ARMv7-M
 */


#include <arch/cache.h>

void tlb_invalidate_all(void)
{
}

void dcache_clean_all(void)
{
}

void dcache_clean_invalidate_all(void)
{
}

void dcache_invalidate_all(void)
{
}

unsigned int dcache_line_bytes(void)
{
	return 0;
}

void dcache_clean_by_mva(void const *addr, size_t len)
{
}

void dcache_clean_invalidate_by_mva(void const *addr, size_t len)
{
}

void dcache_invalidate_by_mva(void const *addr, size_t len)
{
}

void dcache_mmu_disable(void)
{
}

void dcache_mmu_enable(void)
{
}

void cache_sync_instructions(void)
{
}
