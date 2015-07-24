/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google, Inc.
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

#ifndef __MIPS_ARCH_MMU_H
#define __MIPS_ARCH_MMU_H

#include <arch/cpu.h>
#include <stddef.h>
#include <stdint.h>

static inline void tlb_write_indexed(void)
{
	__asm__ __volatile__(
		".set noreorder\n\t"
		"tlbwi\n\t"
		".set reorder");
}

static inline uint32_t get_max_pagesize(void)
{
	uint32_t max_pgsize;

	write_c0_pagemask(C0_PAGEMASK_MASK << C0_PAGEMASK_SHIFT);
	back_to_back_c0_hazard();
	max_pgsize = (((read_c0_pagemask() >> C0_PAGEMASK_SHIFT) &
		       C0_PAGEMASK_MASK) + 1) * 4 * KiB;

	return max_pgsize;
}

static inline uint32_t get_tlb_size(void)
{
	uint32_t tlbsize;

	tlbsize = ((read_c0_config1() >> C0_CONFIG1_MMUSIZE_SHIFT) &
		   C0_CONFIG1_MMUSIZE_MASK) + 1;

	return tlbsize;
}

int identity_map(uint32_t start, size_t len, uint32_t coherency);

#endif /* __MIPS_ARCH_MMU_H */
