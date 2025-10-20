/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RISCV64_CACHE_H
#define RISCV64_CACHE_H

#include <stddef.h>
#include <stdint.h>

/* tlb invalidate all */
void tlb_invalidate_all(void);

void dcache_clean_invalidate_all(void);

/* Invalidate all of the instruction cache for PE to PoU. */
static inline void icache_invalidate_all(void)
{
	__asm__ __volatile__("fence.i\n\t");
}


#endif /* RISCV64_CACHE_H */
