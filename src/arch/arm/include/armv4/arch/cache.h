/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * cache.h: Cache maintenance API for ARM
 */

#ifndef ARM_CACHE_H
#define ARM_CACHE_H

#include <stddef.h>

/*
 * Cache maintenance API
 */

/* dcache clean and invalidate all (on current level given by CCSELR) */
void dcache_clean_invalidate_all(void);

/* dcache clean by modified virtual address to PoC */
void dcache_clean_by_mva(void const *addr, size_t len);

/* dcache clean and invalidate by modified virtual address to PoC */
void dcache_clean_invalidate_by_mva(void const *addr, size_t len);

/* dcache invalidate by modified virtual address to PoC */
void dcache_invalidate_by_mva(void const *addr, size_t len);

void dcache_clean_all(void);

/* dcache invalidate all (on current level given by CCSELR) */
void dcache_invalidate_all(void);

/* returns number of bytes per cache line */
unsigned int dcache_line_bytes(void);

/* dcache and MMU disable */
void dcache_mmu_disable(void);

/* dcache and MMU enable */
void dcache_mmu_enable(void);

/* perform all icache/dcache maintenance needed after loading new code */
void cache_sync_instructions(void);

/* tlb invalidate all */
void tlb_invalidate_all(void);

#endif /* ARM_CACHE_H */
