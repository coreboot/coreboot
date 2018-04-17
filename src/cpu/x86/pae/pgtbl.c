/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Yinghai Lu
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

#include <compiler.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <arch/cpu.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <rules.h>
#include <string.h>

void paging_enable_pae_cr3(uintptr_t cr3)
{
	/* Load the page table address */
	write_cr3(cr3);
	paging_enable_pae();
}

void paging_enable_pae(void)
{
	CRx_TYPE cr0;
	CRx_TYPE cr4;

	/* Enable PAE */
	cr4 = read_cr4();
	cr4 |= CR4_PAE;
	write_cr4(cr4);

	/* Enable Paging */
	cr0 = read_cr0();
	cr0 |= CR0_PG;
	write_cr0(cr0);
}

void paging_disable_pae(void)
{
	CRx_TYPE cr0;
	CRx_TYPE cr4;

	/* Disable Paging */
	cr0 = read_cr0();
	cr0 &= ~(CRx_TYPE)CR0_PG;
	write_cr0(cr0);

	/* Disable PAE */
	cr4 = read_cr4();
	cr4 &= ~(CRx_TYPE)CR4_PAE;
	write_cr4(cr4);
}

#if ENV_RAMSTAGE
void *map_2M_page(unsigned long page)
{
	struct pde {
		uint32_t addr_lo;
		uint32_t addr_hi;
	} __packed;
	struct pg_table {
		struct pde pd[2048];
		struct pde pdp[512];
	} __packed;

	static struct pg_table pgtbl[CONFIG_MAX_CPUS]
		__attribute__((aligned(4096)));
	static unsigned long mapped_window[CONFIG_MAX_CPUS];
	unsigned long index;
	unsigned long window;
	void *result;
	int i;
	index = cpu_index();
	if (index >= CONFIG_MAX_CPUS)
		return MAPPING_ERROR;
	window = page >> 10;
	if (window != mapped_window[index]) {
		paging_disable_pae();
		if (window > 1) {
			struct pde *pd, *pdp;
			/* Point the page directory pointers at the page
			 * directories
			 */
			memset(&pgtbl[index].pdp, 0, sizeof(pgtbl[index].pdp));
			pd = pgtbl[index].pd;
			pdp = pgtbl[index].pdp;
			pdp[0].addr_lo = ((uint32_t)&pd[512*0])|1;
			pdp[1].addr_lo = ((uint32_t)&pd[512*1])|1;
			pdp[2].addr_lo = ((uint32_t)&pd[512*2])|1;
			pdp[3].addr_lo = ((uint32_t)&pd[512*3])|1;
			/* The first half of the page table is identity mapped
			 */
			for (i = 0; i < 1024; i++) {
				pd[i].addr_lo = ((i & 0x3ff) << 21) | 0xE3;
				pd[i].addr_hi = 0;
			}
			/* The second half of the page table holds the mapped
			 * page
			 */
			for (i = 1024; i < 2048; i++) {
				pd[i].addr_lo = ((window & 1) << 31)
					| ((i & 0x3ff) << 21) | 0xE3;
				pd[i].addr_hi = (window >> 1);
			}
			paging_enable_pae_cr3((uintptr_t)pdp);
		}
		mapped_window[index] = window;
	}
	if (window == 0)
		result = (void *)(page << 21);
	else
		result = (void *)(0x80000000 | ((page & 0x3ff) << 21));
	return result;
}
#endif

void paging_set_nxe(int enable)
{
	msr_t msr = rdmsr(IA32_EFER);

	if (enable)
		msr.lo |= EFER_NXE;
	else
		msr.lo &= ~EFER_NXE;

	wrmsr(IA32_EFER, msr);
}

void paging_set_pat(uint64_t pat)
{
	msr_t msr;
	msr.lo = pat;
	msr.hi = pat >> 32;
	wrmsr(MSR_IA32_PAT, msr);
}

/* PAT encoding used in util/x86/x86_page_tables.go. It matches the linux
 * kernel settings:
 *  PTE encoding:
 *      PAT
 *      |PCD
 *      ||PWT  PAT
 *      |||    slot
 *      000    0    WB : _PAGE_CACHE_MODE_WB
 *      001    1    WC : _PAGE_CACHE_MODE_WC
 *      010    2    UC-: _PAGE_CACHE_MODE_UC_MINUS
 *      011    3    UC : _PAGE_CACHE_MODE_UC
 *      100    4    WB : Reserved
 *      101    5    WP : _PAGE_CACHE_MODE_WP
 *      110    6    UC-: Reserved
 *      111    7    WT : _PAGE_CACHE_MODE_WT
 */
void paging_set_default_pat(void)
{
	uint64_t pat =  PAT_ENCODE(WB, 0) | PAT_ENCODE(WC, 1) |
			PAT_ENCODE(UC_MINUS, 2) | PAT_ENCODE(UC, 3) |
			PAT_ENCODE(WB, 4) | PAT_ENCODE(WP, 5) |
			PAT_ENCODE(UC_MINUS, 6) | PAT_ENCODE(WT, 7);
	paging_set_pat(pat);
}
