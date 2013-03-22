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

#include <types.h>
#include <stdlib.h>

#include <cbmem.h>
#include <console/console.h>

#include <arch/cache.h>

#define L1_TLB_ENTRIES	4096

void mmu_setup_by_mva(unsigned long dram_start_mb, unsigned long dram_size_mb)
{
	int i;
	uintptr_t ttb_addr;
	unsigned int ttb_size;
	uint32_t *p;
	uint32_t attr;
	uint32_t ttbcr;

	/*
	 * For coreboot's purposes, we will create a simple L1 page table
	 * in RAM with 1MB section translation entries over the 4GB address
	 * space.
	 * (ref: section 10.2 and example 15-4 in Cortex-A series
	 * programmer's guide)
	 *
	 * FIXME: TLB needs to be aligned to 16KB, but cbmem_add() aligns to
	 * 512 bytes. So add double the space in cbmem and fix-up the pointer.
	 */
	ttb_size = L1_TLB_ENTRIES * sizeof(int);
	ttb_addr = (uintptr_t)cbmem_add(CBMEM_ID_GDT, ttb_size * 2);
	ttb_addr = ALIGN(ttb_addr + ttb_size, ttb_size);
	p = (uint32_t *)ttb_addr;

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
	printk(BIOS_DEBUG, "%s: Writing page table @ to 0x%p\n", __func__, p);
	printk(BIOS_DEBUG, "%s: Non-cachable region: page table @ to 0x%p\n", __func__, p);

	/*
	 * Non-DRAM pages are non-cacheable. Since this may correspond to read-
	 * sensitive regions (ie MMIO), mark as XN to disable speculative
	 * prefetching.
	 */
	attr = (0x3 << 10) | (1 << 4) | 0x2;
	for (i = 0; i < dram_start_mb; i++)
		p[i] = (i << 20) | attr;

	for (i = dram_start_mb + dram_size_mb; i < L1_TLB_ENTRIES; i++)
		p[i] = (i << 20) | attr;

	/* DRAM portion will be write-back for coreboot */
	attr =  (0x3 << 10) | (1 << 4) | (1 << 3) | (1 << 2) | 0x2;
	for (i = dram_start_mb; i < dram_size_mb; i++)
		p[i] = (i << 20) | attr;

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
