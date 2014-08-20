/*
 * This file is part of the coreboot project.
 *
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
#include <config.h>
#include <stdlib.h>
#include <stdint.h>
#include <symbols.h>

#include <cbmem.h>
#include <console/console.h>

#include <arch/cache.h>
#include <arch/io.h>

#if CONFIG_ARM_LPAE
/* See B3.6.2 of ARMv7 Architecture Reference Manual */
/* TODO: Utilize the contiguous hint flag */
#define ATTR_BASE (\
	0ULL << 54 |	/* XN. 0:Not restricted */ \
	0ULL << 53 |	/* PXN. 0:Not restricted */ \
	1 << 10 |	/* AF. 1:Accessed. This is to prevent access \
	 	 	 * fault when accessed for the first time */ \
	0 << 6 |	/* AP[2:1]. 0b00:full access from PL1 */ \
	0 << 5 |	/* NS. 0:Output address is in Secure space */ \
	0 << 1 | 	/* block/table. 0:block entry */ \
	1 << 0		/* validity. 1:valid */ \
	)
#define ATTR_NC		(ATTR_BASE | (MAIR_INDX_NC << 2) | \
			(1ULL << 53) | (1ULL << 54))
#define ATTR_WT		(ATTR_BASE | (MAIR_INDX_WT << 2))
#define ATTR_WB		(ATTR_BASE | (MAIR_INDX_WB << 2))

#define BLOCK_SHIFT	21

typedef uint64_t pgd_t;
typedef uint64_t pmd_t;
static const unsigned int denom = 2;
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
#define ATTR_BASE	((3 << 10) | 0x2)
#define ATTR_NC		(ATTR_BASE | (1 << 4))
#define ATTR_WT		(ATTR_BASE | (1 << 3))
#define ATTR_WB		(ATTR_BASE | (1 << 3) | (1 << 2))

#define BLOCK_SHIFT	20

typedef uint32_t pgd_t;
typedef uint32_t pmd_t;
static const unsigned int denom = 1;
#endif	/* CONFIG_ARM_LPAE */

static pmd_t *const ttb_buff = (pmd_t *)_ttb;

/*
 * mask/shift/size for pages and blocks
 */
#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	~((1UL << PAGE_SHIFT) - 1)
#define BLOCK_SIZE	(1UL << BLOCK_SHIFT)

/*
 * MAIR Index
 */
#define MAIR_INDX_NC	0
#define MAIR_INDX_WT	1
#define MAIR_INDX_WB	2

static void mmu_flush_page_table_entry_range(
		unsigned long start_mb, unsigned long size_mb)
{
	int i;

	/* Flush the page table entries from the dcache. */
	for (i = start_mb/denom; i*denom < start_mb + size_mb; i++)
		dccmvac((uintptr_t)&ttb_buff[i]);
	dsb();
	/* Invalidate the TLB entries. */
	for (i = start_mb/denom; i*denom < start_mb + size_mb; i++)
		tlbimvaa(i*denom*MiB);
	dsb();
	isb();
}

void mmu_disable_range(unsigned long start_mb, unsigned long size_mb)
{
	int i;

	printk(BIOS_DEBUG, "Disabling: [0x%08lx:0x%08lx)\n",
			start_mb*MiB, start_mb*MiB + size_mb*MiB);

	for (i = start_mb/denom; i*denom < start_mb + size_mb; i++)
		ttb_buff[i] = 0;

	mmu_flush_page_table_entry_range(start_mb, size_mb);
}

void mmu_config_range(unsigned long start_mb, unsigned long size_mb,
		enum dcache_policy policy)
{
	const char *str = NULL;
	pmd_t attr;
	int i;

	switch(policy) {
	case DCACHE_OFF:
		/* XN set to avoid prefetches to uncached/unbuffered regions */
		attr = ATTR_NC;
		str = "off";
		break;
	case DCACHE_WRITEBACK:
		attr = ATTR_WB;
		str = "writeback";
		break;
	case DCACHE_WRITETHROUGH:
		attr = ATTR_WT;
		str = "writethrough";
		break;
	default:
		printk(BIOS_ERR, "unknown dcache policy: %02x\n", policy);
		return;
	}

	printk(BIOS_DEBUG, "Setting dcache policy: [0x%08lx:0x%08lx) [%s]\n",
			start_mb << 20, ((start_mb + size_mb) << 20), str);

	/* Write out page table entries. */
	for (i = start_mb/denom; i*denom < start_mb + size_mb; i++)
		ttb_buff[i] = ((pmd_t)i << BLOCK_SHIFT) | attr;

	mmu_flush_page_table_entry_range(start_mb, size_mb);
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
        if (CONFIG_ARM_LPAE) {
                pgd_t *const pgd_buff = (pgd_t*)(_ttb + 16*KiB);
                pmd_t *pmd = ttb_buff;
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
                        pgd_buff[i] = ((uint32_t)pmd & PAGE_MASK) |
                                3;	/* 0b11: valid table entry */
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

	/* disable domain-level checking of permissions */
	write_dacr(~0);
}
