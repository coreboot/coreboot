/*
 * Early initialization code for riscv virtual memory
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <vm.h>
#include <arch/encoding.h>
#include <atomic.h>
#include <stdint.h>
#include <console/console.h>

pte_t* root_page_table;

void walk_page_table(void) {
	// TODO: implement a full walk to make sure memory was set up
	//const size_t pte_per_page = RISCV_PGSIZE/sizeof(void*);
	pte_t* t = root_page_table;
	printk(BIOS_DEBUG, "root_page_table: %p\n", t);
}

void enter_supervisor(void) {
	// enter supervisor mode
	asm volatile("la t0, 1f; csrw mepc, t0; eret; 1:" ::: "t0");
}

void flush_tlb(void)
{
	asm volatile("sfence.vm");
}

size_t pte_ppn(pte_t pte)
{
	return pte >> PTE_PPN_SHIFT;
}

pte_t ptd_create(uintptr_t ppn)
{
	return (ppn << PTE_PPN_SHIFT) | PTE_V | PTE_TYPE_TABLE;
}

pte_t pte_create(uintptr_t ppn, int prot, int user)
{
	pte_t pte = (ppn << PTE_PPN_SHIFT) | PTE_V;
	if (prot & PROT_WRITE) pte |= PTE_TYPE_URW_SRW;
	if (prot & PROT_EXEC) pte |= PTE_TYPE_URX_SRX;
	if (!user) pte |= PTE_TYPE_SR;
	return pte;
}

void init_vm(uintptr_t virtMemStart, uintptr_t physMemStart, uintptr_t pageTableStart) {
	pte_t* sbi_pt = (pte_t*) pageTableStart;
	memset(sbi_pt, 0, RISCV_PGSIZE);
	// need to leave room for sbi page
	uintptr_t memorySize = 0x7F000000; // 0xFFF... - 0xFFFFFFFF81000000 - RISCV_PGSIZE

	// middle page table
	pte_t* middle_pt = (void*)sbi_pt + RISCV_PGSIZE;
	size_t num_middle_pts = 2; // 3 level page table, 39 bit virtual address space for now

	// root page table
	pte_t* root_pt = (void*)middle_pt + num_middle_pts * RISCV_PGSIZE;
	memset(middle_pt, 0, (num_middle_pts + 1) * RISCV_PGSIZE); // 0's out middle_pt and root_pt
	for (size_t i = 0; i < num_middle_pts; i++)
		root_pt[(1<<RISCV_PGLEVEL_BITS)-num_middle_pts+i] = ptd_create(((uintptr_t)middle_pt >> RISCV_PGSHIFT) + i);

	// fill the middle page table
	for (uintptr_t vaddr = virtMemStart, paddr = physMemStart; paddr < memorySize; vaddr += SUPERPAGE_SIZE, paddr += SUPERPAGE_SIZE) {
		int l2_shift = RISCV_PGLEVEL_BITS + RISCV_PGSHIFT;
		size_t l2_idx = (virtMemStart >> l2_shift) & ((1 << RISCV_PGLEVEL_BITS)-1);
		l2_idx += ((vaddr - virtMemStart) >> l2_shift);
		middle_pt[l2_idx] = pte_create(paddr >> RISCV_PGSHIFT, PROT_READ|PROT_WRITE|PROT_EXEC, 0);
	}

	// map SBI at top of vaddr space
	uintptr_t num_sbi_pages = 1; // only need to map a single page for sbi interface
	uintptr_t sbiStartAddress = 0x2000; // the start of the sbi mapping
	uintptr_t sbiAddr = sbiStartAddress;
	for (uintptr_t i = 0; i < num_sbi_pages; i++) {
		uintptr_t idx = (1 << RISCV_PGLEVEL_BITS) - num_sbi_pages + i;
		sbi_pt[idx] = pte_create(sbiAddr >> RISCV_PGSHIFT, PROT_READ|PROT_EXEC, 0);
		sbiAddr += RISCV_PGSIZE;
	}
	pte_t* sbi_pte = middle_pt + ((num_middle_pts << RISCV_PGLEVEL_BITS)-1);
	*sbi_pte = ptd_create((uintptr_t)sbi_pt >> RISCV_PGSHIFT);

	mb();
	root_page_table = root_pt;
	write_csr(sptbr, root_pt);
}

void initVirtualMemory(void) {
	printk(BIOS_DEBUG, "Initializing virtual memory...\n");
	uintptr_t physicalStart = 0x1000000; // TODO: Figure out how to grab this from cbfs
	uintptr_t virtualStart = 0xffffffff81000000;
	uintptr_t pageTableStart = 0x1400000;
	init_vm(virtualStart, physicalStart, pageTableStart);
	mb();
	printk(BIOS_DEBUG, "Finished initializing virtual memory, starting walk...\n");
	walk_page_table();
}

void mstatus_init(void)
{
	// supervisor support is required

	uintptr_t ms = 0;
	ms = INSERT_FIELD(ms, MSTATUS_PRV, PRV_M);
	ms = INSERT_FIELD(ms, MSTATUS_PRV1, PRV_S);
	ms = INSERT_FIELD(ms, MSTATUS_PRV2, PRV_U);
	ms = INSERT_FIELD(ms, MSTATUS_IE2, 1);
	ms = INSERT_FIELD(ms, MSTATUS_VM, VM_CHOICE);
	ms = INSERT_FIELD(ms, MSTATUS_FS, 3);
	ms = INSERT_FIELD(ms, MSTATUS_XS, 3);
	write_csr(mstatus, ms);
	ms = read_csr(mstatus);

	if (EXTRACT_FIELD(ms, MSTATUS_VM) != VM_CHOICE) {
		printk(BIOS_DEBUG, "we don't have virtual memory...\n");
	} else {
		printk(BIOS_DEBUG, "-----------------------------\n");
		printk(BIOS_DEBUG, "virtual memory status enabled\n");
		printk(BIOS_DEBUG, "-----------------------------\n");
	}

	clear_csr(mip, MIP_MSIP);
	set_csr(mie, MIP_MSIP);
}
