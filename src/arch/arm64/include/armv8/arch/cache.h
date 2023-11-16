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

struct cache_info {
	uint64_t size;          // total size of cache in bytes
	uint64_t associativity; // number of cache lines in a set
	uint64_t numsets;       // number of sets in a cache
	uint8_t  line_bytes;    // size of cache line in bytes
};

enum cache_type cpu_get_cache_type(enum cache_level level);
enum cb_err cpu_get_cache_info(const enum cache_level level, const enum cache_type type,
			       struct cache_info *info);

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

	tlbiall();
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
