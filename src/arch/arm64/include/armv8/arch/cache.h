/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * cache.h: Cache maintenance API for ARM64
 */

#ifndef ARM_ARM64_CACHE_H
#define ARM_ARM64_CACHE_H

#include <arch/lib_helpers.h>

#ifndef __ASSEMBLER__

#include <stddef.h>
#include <arch/barrier.h>

enum cache_level {
	CACHE_L1 = 1,
	CACHE_L2 = 2,
	CACHE_L3 = 3,
	CACHE_L4 = 4,
	CACHE_L5 = 5,
	CACHE_L6 = 6,
	CACHE_L7 = 7,
};

enum cache_type {
	NO_CACHE = 0,
	CACHE_INSTRUCTION = 1,
	CACHE_DATA = 2,
	CACHE_SEPARATE = 3,
	CACHE_UNIFIED = 4,
};

enum cache_type cpu_get_cache_type(enum cache_level level);
void cpu_get_cache_info(enum cache_level level, enum cache_type, size_t *cache_size, size_t *assoc);

/* dcache clean by virtual address to PoC */
void dcache_clean_by_mva(void const *addr, size_t len);

/* dcache clean and invalidate by virtual address to PoC */
void dcache_clean_invalidate_by_mva(void const *addr, size_t len);

/* dcache invalidate by virtual address to PoC */
void dcache_invalidate_by_mva(void const *addr, size_t len);

/* dcache clean and/or invalidate all sets/ways to PoC */
void dcache_clean_all(void);
void dcache_invalidate_all(void);
void dcache_clean_invalidate_all(void);

/* returns number of bytes per cache line */
unsigned int dcache_line_bytes(void);

/* Invalidate all TLB entries. */
static inline void tlb_invalidate_all(void)
{
	/* TLBIALL includes dTLB and iTLB on systems that have them. */
	tlbiall_el3();
	dsb();
	isb();
}

/* Invalidate all of the instruction cache for PE to PoU. */
static inline void icache_invalidate_all(void)
{
	dsb();
	iciallu();
	dsb();
	isb();
}

#endif /* __ASSEMBLER__ */

#endif /* ARM_ARM64_CACHE_H */
