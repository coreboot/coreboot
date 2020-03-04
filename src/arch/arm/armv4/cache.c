/* SPDX-License-Identifier: BSD-3-Clause */
/* This file is part of the coreboot project. */
/*
 * cache.c: Cache maintenance routines for ARMv7-A and ARMv7-R
 *
 * Reference: ARM Architecture Reference Manual, ARMv7-A and ARMv7-R edition
 */

#include <stdint.h>

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
	/*
	 * TODO: Implement this correctly. For now we just return a
	 * reasonable value. It was added during Nyan development and
	 * may be used in bootblock code. It matters only if dcache is
	 * turned on.
	 */
	return 64;
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
