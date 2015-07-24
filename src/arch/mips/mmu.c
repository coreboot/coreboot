/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/mmu.h>
#include <console/console.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MIN_PAGE_SIZE (4 * KiB)

static int add_wired_tlb_entry(uint32_t entrylo0, uint32_t entrylo1,
			       uint32_t entryhi, uint32_t pgsize)
{
	uint32_t tlbindex;

	tlbindex = read_c0_wired();
	if (tlbindex >= get_tlb_size() || tlbindex >= C0_WIRED_MASK) {
		printk(BIOS_ERR, "Ran out of TLB entries\n");
		return -1;
	}
	write_c0_wired(tlbindex + 1);
	write_c0_index(tlbindex);
	write_c0_pagemask(((pgsize / MIN_PAGE_SIZE) - 1) << C0_PAGEMASK_SHIFT);
	write_c0_entryhi(entryhi);
	write_c0_entrylo0(entrylo0);
	write_c0_entrylo1(entrylo1);
	mtc0_tlbw_hazard();
	tlb_write_indexed();
	tlbw_use_hazard();

	return 0;
}

static uint32_t pick_pagesize(uint32_t start, uint32_t len)
{
	uint32_t pgsize, max_pgsize;

	max_pgsize = get_max_pagesize();
	for (pgsize = max_pgsize;
	     pgsize >= MIN_PAGE_SIZE;
	     pgsize = pgsize / 4) {
		/*
		 * Each TLB entry maps a pair of virtual pages.  To avoid
		 * aliasing, pick the largest page size that is at most
		 * half the size of the region we're trying to map.
		 */
		if (IS_ALIGNED(start, 2 * pgsize) && (2 * pgsize <= len))
			break;
	}

	return pgsize;
}

/*
 * Identity map the memory from [start,start+len] in the TLB using the
 * largest suitable page size so as to conserve TLB entries.
 */
int identity_map(uint32_t start, size_t len, uint32_t coherency)
{
	uint32_t pgsize, pfn, entryhi, entrylo0, entrylo1;

	coherency &= C0_ENTRYLO_COHERENCY_MASK;
	while (len > 0) {
		pgsize = pick_pagesize(start, len);
		entryhi = start;
		pfn = start >> 12;
		entrylo0 = (pfn << C0_ENTRYLO_PFN_SHIFT) | coherency |
			C0_ENTRYLO_D | C0_ENTRYLO_V | C0_ENTRYLO_G;
		start += pgsize;
		len -= MIN(len, pgsize);
		if (len >= pgsize) {
			pfn = start >> 12;
			entrylo1 = (pfn << C0_ENTRYLO_PFN_SHIFT) |
				coherency | C0_ENTRYLO_D | C0_ENTRYLO_V |
				C0_ENTRYLO_G;
			start += pgsize;
			len -= MIN(len, pgsize);
		} else {
			entrylo1 = 0;
		}
		if (add_wired_tlb_entry(entrylo0, entrylo1, entryhi, pgsize))
			return -1;
	}

	return 0;
}
