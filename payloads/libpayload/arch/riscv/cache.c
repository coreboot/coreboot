/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>

void tlb_invalidate_all(void)
{
	asm volatile ("sfence.vma" : : : "memory");
}

void dcache_clean_invalidate_all(void)
{

}
