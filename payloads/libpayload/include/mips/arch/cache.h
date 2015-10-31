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

#ifndef __MIPS_ARCH_CACHE_H__
#define __MIPS_ARCH_CACHE_H__


#include <stddef.h>
#include <stdint.h>


/*
 * Sync primitives
 */

/* data memory barrier */
static inline void dmb(void)
{
	/* TODO */
}

/* data sync barrier */
static inline void dsb(void)
{
	/* TODO */
}

/* instruction sync barrier */
static inline void isb(void)
{
	/* TODO */
}


/*
 * Cache maintenance API
 */

/* dcache clean and invalidate all */
void dcache_clean_invalidate_all(void);

/* dcache clean all */
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

/*
 * Generalized setup/init functions
 */

/* mmu initialization (set page table address, set permissions, etc) */
void mmu_init(void);

enum dcache_policy {
	DCACHE_OFF,
	DCACHE_WRITEBACK,
	DCACHE_WRITETHROUGH,
};

/* disable the mmu for a range. Primarily useful to lock out address 0. */
void mmu_disable_range(unsigned long start_mb, unsigned long size_mb);
/* mmu range configuration (set dcache policy) */
void mmu_config_range(unsigned long start_mb, unsigned long size_mb,
						enum dcache_policy policy);

#endif /* __MIPS_ARCH_CACHE_H__ */
