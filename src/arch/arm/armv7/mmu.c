/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 * Copyright 2013 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <symbols.h>

#include <cbmem.h>
#include <console/console.h>

#include <arch/cache.h>
#include <arch/io.h>

#if IS_ENABLED(CONFIG_ARM_LPAE)
/* See B3.6.2 of ARMv7 Architecture Reference Manual */
/* TODO: Utilize the contiguous hint flag */
#define ATTR_BLOCK (\
	0ULL << 54 |	/* XN. 0:Not restricted */ \
	0ULL << 53 |	/* PXN. 0:Not restricted */ \
	1 << 10 |	/* AF. 1:Accessed. This is to prevent access */ \
			/* fault when accessed for the first time */ \
	0 << 6 |	/* AP[2:1]. 0b00:full access from PL1 */ \
	0 << 5 |	/* NS. 0:Output address is in Secure space */ \
	0 << 1 |	/* block/table. 0:block entry */ \
	1 << 0		/* validity. 1:valid */ \
	)
#define ATTR_PAGE	(ATTR_BLOCK | 1 << 1)
#define ATTR_NEXTLEVEL	(0x3)
#define ATTR_NC		((MAIR_INDX_NC << 2) | (1ULL << 53) | (1ULL << 54))
#define ATTR_WT		(MAIR_INDX_WT << 2)
#define ATTR_WB		(MAIR_INDX_WB << 2)

#define PAGE_MASK	0x000ffffffffff000ULL
#define BLOCK_MASK	0x000fffffffe00000ULL
#define NEXTLEVEL_MASK	PAGE_MASK
#define BLOCK_SHIFT	21

typedef uint64_t pte_t;
#else	/* CONFIG_ARM_LPAE */
	/*
	 * Section entry bits:
	 * 31:20 - section base address
	 *    18 - 0 to indicate normal section (versus supersection)
	 *    17 - nG, 0 to indicate page is global
	 *    16 - S, 0 for non-shareable (?)
	 *    15 - APX, 0 for full access
	 * 14:12 - TEX, 0b000 for outer and inner write-back
	 * 11:10 - AP, 0b11 for full access
	 *     9 - P, ? (FIXME: not described or possibly obsolete?)
	 *  8: 5 - Domain
	 *     4 - XN, 1 to set execute-never (and also avoid prefetches)
	 *     3 - C, 1 for cacheable
	 *     2 - B, 1 for bufferable
	 *  1: 0 - 0b10 to indicate section entry
	 */
#define ATTR_BLOCK	((3 << 10) | 0x2)
#define ATTR_PAGE	((3 << 4) | 0x2)
#define ATTR_NEXTLEVEL	(0x1)
#define ATTR_NC		(1 << 4)
#define ATTR_WT		(1 << 3)
#define ATTR_WB		((1 << 3) | (1 << 2))

#define PAGE_MASK	0xfffff000UL
#define BLOCK_MASK	0xfff00000UL
#define NEXTLEVEL_MASK	0xfffffc00UL
#define BLOCK_SHIFT	20

typedef uint32_t pte_t;
#endif	/* CONFIG_ARM_LPAE */

/* We set the first PTE to a sentinel value that cannot occur naturally (has
 * attributes set but bits [1:0] are 0 -> unmapped) to mark unused subtables. */
#define ATTR_UNUSED	0xBADbA6E0
#define SUBTABLE_PTES	(1 << (BLOCK_SHIFT - PAGE_SHIFT))

/*
 * mask/shift/size for pages and blocks
 */
#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define BLOCK_SIZE	(1UL << BLOCK_SHIFT)

/*
 * MAIR Index
 */
#define MAIR_INDX_NC	0
#define MAIR_INDX_WT	1
#define MAIR_INDX_WB	2

static pte_t *const ttb_buff = (void *)_ttb;

/* Not all boards want to use subtables and declare them in memlayout.ld. */
DECLARE_OPTIONAL_REGION(ttb_subtables);

static struct {
	pte_t value;
	const char *name;
} attrs[] = {
	[DCACHE_OFF] = {.value = ATTR_NC, .name = "uncached"},
	[DCACHE_WRITEBACK] = {.value = ATTR_WB, .name = "writeback"},
	[DCACHE_WRITETHROUGH] = {.value = ATTR_WT, .name = "writethrough"},
};

/* Fills page table entries in |table| from |start_idx| to |end_idx| with |attr|
 * and performs necessary invalidations. |offset| is the start address of the
 * area described by |table|, and |shift| is the size-shift of each frame. */
static void mmu_fill_table(pte_t *table, u32 start_idx, u32 end_idx,
			   uintptr_t offset, u32 shift, pte_t attr)
{
	int i;

	/* Write out page table entries. */
	for (i = start_idx; i < end_idx; i++)
		table[i] = (offset + (i << shift)) | attr;

	/* Flush the page table entries from the dcache. */
	for (i = start_idx; i < end_idx; i++)
		dccmvac((uintptr_t)&table[i]);
	dsb();

	/* Invalidate the TLB entries. */
	for (i = start_idx; i < end_idx; i++)
		tlbimvaa(offset + (i << shift));
	dsb();
	isb();
}

static pte_t *mmu_create_subtable(pte_t *pgd_entry)
{
	pte_t *table = (pte_t *)_ttb_subtables;

	/* Find unused subtable (first PTE == ATTR_UNUSED). */
	while (table[0] != ATTR_UNUSED) {
		table += SUBTABLE_PTES;
		if ((pte_t *)_ettb_subtables - table <= 0)
			die("Not enough room for another sub-pagetable!");
	}

	/* We assume that *pgd_entry must already be a valid block mapping. */
	uintptr_t start_addr = (uintptr_t)(*pgd_entry & BLOCK_MASK);
	printk(BIOS_DEBUG, "Creating new subtable @%p for [%#.8x:%#.8lx)\n",
	       table, start_addr, start_addr + BLOCK_SIZE);

	/* Initialize the new subtable with entries of the same attributes
	 * (XN bit moves from 4 to 0, set PAGE unless block was unmapped). */
	pte_t attr = *pgd_entry & ~(BLOCK_MASK);
	if (!IS_ENABLED(CONFIG_ARM_LPAE) && (attr & (1 << 4)))
		attr = ((attr & ~(1 << 4)) | (1 << 0));
	if (attr & ATTR_BLOCK)
		attr = (attr & ~ATTR_BLOCK) | ATTR_PAGE;
	mmu_fill_table(table, 0, SUBTABLE_PTES, start_addr, PAGE_SHIFT, attr);

	/* Replace old entry in upper level table to point at subtable. */
	*pgd_entry = (pte_t)(uintptr_t)table | ATTR_NEXTLEVEL;
	dccmvac((uintptr_t)pgd_entry);
	dsb();
	tlbimvaa(start_addr);
	dsb();
	isb();

	return table;
}

static pte_t *mmu_validate_create_sub_table(u32 start_kb, u32 size_kb)
{
	pte_t *pgd_entry = &ttb_buff[start_kb / (BLOCK_SIZE/KiB)];
	pte_t *table = (void *)(uintptr_t)(*pgd_entry & NEXTLEVEL_MASK);

	/* Make sure the range is contained within a single superpage. */
	assert(((start_kb + size_kb - 1) & (BLOCK_MASK/KiB))
	       == (start_kb & (BLOCK_MASK/KiB)) && start_kb < 4 * (GiB/KiB));

	if ((*pgd_entry & ~NEXTLEVEL_MASK) != ATTR_NEXTLEVEL)
		table = mmu_create_subtable(pgd_entry);

	return table;
}

void mmu_config_range_kb(u32 start_kb, u32 size_kb, enum dcache_policy policy)
{
	pte_t *table = mmu_validate_create_sub_table(start_kb, size_kb);

	/* Always _one_ _damn_ bit that won't fit... (XN moves from 4 to 0) */
	pte_t attr = attrs[policy].value;
	if (!IS_ENABLED(CONFIG_ARM_LPAE) && (attr & (1 << 4)))
		attr = ((attr & ~(1 << 4)) | (1 << 0));

	/* Mask away high address bits that are handled by upper level table. */
	u32 mask = BLOCK_SIZE/KiB - 1;
	printk(BIOS_DEBUG, "Mapping address range [%#.8x:%#.8x) as %s\n",
	       start_kb * KiB, (start_kb + size_kb) * KiB, attrs[policy].name);

	u32 end_kb = ALIGN_UP((start_kb + size_kb), PAGE_SIZE/KiB) -
		     (start_kb & ~mask);

	assert(end_kb <= BLOCK_SIZE/KiB);

	mmu_fill_table(table, (start_kb & mask) / (PAGE_SIZE/KiB),
		       end_kb / (PAGE_SIZE/KiB),
		       (start_kb & ~mask) * KiB, PAGE_SHIFT, ATTR_PAGE | attr);
}

void mmu_disable_range_kb(u32 start_kb, u32 size_kb)
{
	pte_t *table = mmu_validate_create_sub_table(start_kb, size_kb);

	/* Mask away high address bits that are handled by upper level table. */
	u32 mask = BLOCK_SIZE/KiB - 1;
	printk(BIOS_DEBUG, "Setting address range [%#.8x:%#.8x) as unmapped\n",
	       start_kb * KiB, (start_kb + size_kb) * KiB);
	mmu_fill_table(table, (start_kb & mask) / (PAGE_SIZE/KiB),
		       div_round_up((start_kb + size_kb) & mask, PAGE_SIZE/KiB),
		       (start_kb & ~mask) * KiB, PAGE_SHIFT, 0);
}

void mmu_disable_range(u32 start_mb, u32 size_mb)
{
	printk(BIOS_DEBUG, "Setting address range [%#.8x:%#.8x) as unmapped\n",
	       start_mb * MiB, (start_mb + size_mb) * MiB);
	assert(start_mb + size_mb <= 4 * (GiB/MiB));
	mmu_fill_table(ttb_buff, start_mb / (BLOCK_SIZE/MiB),
		       div_round_up(start_mb + size_mb, BLOCK_SIZE/MiB),
		       0, BLOCK_SHIFT, 0);
}

void mmu_config_range(u32 start_mb, u32 size_mb, enum dcache_policy policy)
{
	printk(BIOS_DEBUG, "Mapping address range [%#.8x:%#.8x) as %s\n",
	       start_mb * MiB, (start_mb + size_mb) * MiB, attrs[policy].name);
	assert(start_mb + size_mb <= 4 * (GiB/MiB));
	mmu_fill_table(ttb_buff, start_mb / (BLOCK_SIZE/MiB),
		       div_round_up(start_mb + size_mb, BLOCK_SIZE/MiB),
		       0, BLOCK_SHIFT, ATTR_BLOCK | attrs[policy].value);
}

/*
 * For coreboot's purposes, we will create a simple identity map.
 *
 * If LPAE is disabled, we will create a L1 page
 * table in RAM with 1MB section translation entries over the 4GB address space.
 * (ref: section 10.2 and example 15-4 in Cortex-A series programmer's guide)
 *
 * If LPAE is enabled, we do two level translation with one L1 table with 4
 * entries, each covering a 1GB space, and four L2 tables with 512 entries, each
 * covering a 2MB space.
 */
void mmu_init(void)
{
	/* Initially mark all subtables as unused (first PTE == ATTR_UNUSED). */
	pte_t *table = (pte_t *)_ttb_subtables;
	for (; (pte_t *)_ettb_subtables - table > 0; table += SUBTABLE_PTES)
		table[0] = ATTR_UNUSED;

	if (CONFIG_ARM_LPAE) {
		pte_t *const pgd_buff = (pte_t *)(_ttb + 16*KiB);
		pte_t *pmd = ttb_buff;
		int i;

		printk(BIOS_DEBUG, "LPAE Translation tables are @ %p\n",
		       ttb_buff);
		ASSERT((read_mmfr0() & 0xf) >= 5);

		/*
		 * Set MAIR
		 * See B4.1.104 of ARMv7 Architecture Reference Manual
		 */
		write_mair0(
			0x00 << (MAIR_INDX_NC*8) | /* Strongly-ordered,
						    * Non-Cacheable */
			0xaa << (MAIR_INDX_WT*8) | /* Write-Thru,
						    * Read-Allocate */
			0xff << (MAIR_INDX_WB*8)   /* Write-Back,
						    * Read/Write-Allocate */
		);

		/*
		 * Set up L1 table
		 * Once set here, L1 table won't be modified by coreboot.
		 * See B3.6.1 of ARMv7 Architecture Reference Manual
		 */
		for (i = 0; i < 4; i++) {
			pgd_buff[i] = ((uint32_t)pmd & NEXTLEVEL_MASK) |
				      ATTR_NEXTLEVEL;
			pmd += BLOCK_SIZE / PAGE_SIZE;
		}

		/*
		 * Set TTBR0
		 */
		write_ttbr0((uintptr_t)pgd_buff);
	} else {
		printk(BIOS_DEBUG, "Translation table is @ %p\n", ttb_buff);

		/*
		 * Translation table base 0 address is in bits 31:14-N, where N
		 * is given by bits 2:0 in TTBCR (which we set to 0). All lower
		 * bits in this register should be zero for coreboot.
		 */
		write_ttbr0((uintptr_t)ttb_buff);
	}

	/*
	 * Set TTBCR
	 * See B4.1.153 of ARMv7 Architecture Reference Manual
	 * See B3.5.4 and B3.6.4 for how TTBR0 or TTBR1 is selected.
	 */
	write_ttbcr(
		CONFIG_ARM_LPAE << 31 |	/* EAE. 1:Enable LPAE */
		0 << 16 | 0 << 0	/* Use TTBR0 for all addresses */
		);

	/* Set domain 0 to Client so XN bit works (to prevent prefetches) */
	write_dacr(0x5);
}
