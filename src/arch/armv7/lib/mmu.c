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

#include <stdlib.h>
#include <stdint.h>

#include <cbmem.h>
#include <console/console.h>

#include <arch/cache.h>
#include <arch/io.h>

#define L1_TLB_ENTRIES	4096	/* 1 entry for each 1MB address space */

static uintptr_t ttb_addr;

void mmu_disable_range(unsigned long start_mb, unsigned long size_mb)
{
	unsigned int i;
	uint32_t *ttb_entry = (uint32_t *)ttb_addr;
	printk(BIOS_DEBUG, "Disabling: 0x%08lx:0x%08lx\n",
			start_mb*MiB, start_mb*MiB + size_mb*MiB - 1);

	for (i = start_mb; i < start_mb + size_mb; i++)
		writel(0, &ttb_entry[i]);

	for (i = start_mb; i < start_mb + size_mb; i++) {
		dccmvac((uintptr_t)&ttb_entry[i]);
		tlbimvaa(i*MiB);
	}
}

void mmu_config_range(unsigned long start_mb, unsigned long size_mb,
		enum dcache_policy policy)
{
	unsigned int i;
	uint32_t attr;
	uint32_t *ttb_entry = (uint32_t *)ttb_addr;
	const char *str = NULL;

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

	switch(policy) {
	case DCACHE_OFF:
		/* XN set to avoid prefetches to uncached/unbuffered regions */
		attr = (0x3 << 10) | (1 << 4) | 0x2;
		str = "off";
		break;
	case DCACHE_WRITEBACK:
		attr =  (0x3 << 10) | (1 << 3) | (1 << 2) | 0x2;
		str = "writeback";
		break;
	case DCACHE_WRITETHROUGH:
		attr =  (0x3 << 10) | (1 << 3) | (1 << 2) | 0x2;
		str = "writethrough";
		break;
	default:
		printk(BIOS_ERR, "unknown dcache policy: %02x\n", policy);
		return;
	}

	printk(BIOS_DEBUG, "Setting dcache policy: 0x%08lx:0x%08lx [%s]\n",
			start_mb << 20, ((start_mb + size_mb) << 20) - 1, str);

	/* Write out page table entries. */
	for (i = start_mb; i < start_mb + size_mb; i++)
		writel((i << 20) | attr, &ttb_entry[i]);

	/* Flush the page table entries, and old translations from the TLB. */
	for (i = start_mb; i < start_mb + size_mb; i++) {
		dccmvac((uintptr_t)&ttb_entry[i]);
		tlbimvaa(i*MiB);
	}
}

void mmu_init(void)
{
	unsigned int ttb_size;
	uint32_t ttbcr;

	/*
	 * For coreboot's purposes, we will create a simple L1 page table
	 * in RAM with 1MB section translation entries over the 4GB address
	 * space.
	 * (ref: section 10.2 and example 15-4 in Cortex-A series
	 * programmer's guide)
	 *
	 * FIXME: TLB needs to be aligned to 16KB, but cbmem_add() aligns to
	 * 512 bytes. So allocate some extra space in cbmem and fix-up the
	 * pointer.
	 */
	ttb_size = L1_TLB_ENTRIES * sizeof(uint32_t);
	ttb_addr = (uintptr_t)cbmem_add(CBMEM_ID_GDT, ttb_size + 16*KiB);
	ttb_addr = ALIGN(ttb_addr, 16*KiB);
	printk(BIOS_DEBUG, "Translation table is @ 0x%08x\n", ttb_addr);

	/*
	 * Disable TTBR1 by setting TTBCR.N to 0b000, which means the TTBR0
	 * table size is 16KB and has indices VA[31:20].
	 *
	 * ref: Arch Ref. Manual for ARMv7-A, B3.5.4,
	 */
	ttbcr = read_ttbcr();
	ttbcr &= ~(0x3);
	write_ttbcr(ttbcr);

	/*
	 * Translation table base 0 address is in bits 31:14-N, where N is given
	 * by bits 2:0 in TTBCR (which we set to 0). All lower bits in this
	 * register should be zero for coreboot.
	 */
	write_ttbr0(ttb_addr);

	/* disable domain-level checking of permissions */
	write_dacr(~0);
}
