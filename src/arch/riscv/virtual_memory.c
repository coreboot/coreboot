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
 */

#include <arch/barrier.h>
#include <arch/encoding.h>
#include <arch/sbi.h>
#include <atomic.h>
#include <console/console.h>
#include <stdint.h>
#include <vm.h>
#include <symbols.h>

pte_t* root_page_table;

/* Indent the following text by 2*level spaces */
static void indent(int level)
{
	int i;
	for (i = 0; i < level; i++)
		printk(BIOS_DEBUG, "  ");
}

/*
 * Convert a page table index at a given page table level to a virtual address
 * offset
 */
static uintptr_t index_to_virt_addr(int index, int level)
{
	/*
	 * Index is at most RISCV_PGLEVEL_BITS bits wide (not considering the
	 * leading zeroes. If level==0, the below expression thus shifts index
	 * into the highest bits of a 64-bit number, and then shifts it down
	 * with sign extension.
	 *
	 * If level>0, then the expression should work as expected, without any
	 * magic.
	 */
	return ((intptr_t)index)
		<< (64 - RISCV_PGLEVEL_BITS - level * RISCV_PGLEVEL_BITS)
		>> (64 - VA_BITS);
}

/* Dump the page table structures to the console -- helper function */
static void print_page_table_at(pte_t *pt, intptr_t virt_addr, int level)
{
	int i;

	indent(level);
	printk(BIOS_DEBUG, "Level %d page table at 0x%p\n", level, pt);

	for (i = 0; i < RISCV_PGSIZE / sizeof(pte_t); i++) {
		char urwx[8];
		uintptr_t pointer;
		intptr_t next_virt_addr;

		if (!(pt[i] & PTE_V))
			continue;

		urwx[0] = (pt[i] & PTE_U)? 'u' : '-';
		urwx[1] = (pt[i] & PTE_R)? 'r' : '-';
		urwx[2] = (pt[i] & PTE_W)? 'w' : '-';
		urwx[3] = (pt[i] & PTE_X)? 'x' : '-';
		urwx[4] = '\0';

		next_virt_addr = virt_addr + index_to_virt_addr(i, level);

		pointer = ((uintptr_t)pt[i] >> 10) << RISCV_PGSHIFT;

		indent(level + 1);
		printk(BIOS_DEBUG, "Valid PTE at index %d (0x%016zx -> 0x%zx), ",
				i, (size_t) next_virt_addr, (size_t) pointer);
		if (PTE_TABLE(pt[i]))
			printk(BIOS_DEBUG, "page table\n");
		else
			printk(BIOS_DEBUG, "protections %s\n", urwx);

		if (PTE_TABLE(pt[i])) {
			print_page_table_at((pte_t *)pointer, next_virt_addr, level + 1);
		}
	}
}

/* Print the page table structures to the console */
void print_page_table(void) {
	print_page_table_at((void *)(read_csr(sptbr) << RISCV_PGSHIFT), 0, 0);
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
	return (ppn << PTE_PPN_SHIFT) | PTE_V;
}

pte_t pte_create(uintptr_t ppn, int prot, int user)
{
	pte_t pte = (ppn << PTE_PPN_SHIFT) | PTE_R | PTE_V;
	if (prot & PTE_W)
		pte |= PTE_W;
	if (prot & PTE_X)
		pte |= PTE_X;
	if (user)
		pte |= PTE_U;
	return pte;
}

void init_vm(uintptr_t virtMemStart, uintptr_t physMemStart, pte_t *sbi_pt)
{
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
	for (uintptr_t vaddr = virtMemStart, paddr = physMemStart;
			paddr < physMemStart + memorySize;
			vaddr += SUPERPAGE_SIZE, paddr += SUPERPAGE_SIZE) {
		int l2_shift = RISCV_PGLEVEL_BITS + RISCV_PGSHIFT;
		size_t l2_idx = (virtMemStart >> l2_shift) & ((1 << RISCV_PGLEVEL_BITS)-1);
		l2_idx += ((vaddr - virtMemStart) >> l2_shift);
		middle_pt[l2_idx] = pte_create(paddr >> RISCV_PGSHIFT,
					       PTE_U|PTE_R|PTE_W|PTE_X, 0);
	}

	// map SBI at top of vaddr space
	// only need to map a single page for sbi interface
	uintptr_t num_sbi_pages = 1;
	uintptr_t sbiStartAddress = (uintptr_t) &sbi_page;
	uintptr_t sbiAddr = sbiStartAddress;
	for (uintptr_t i = 0; i < num_sbi_pages; i++) {
		uintptr_t idx = (1 << RISCV_PGLEVEL_BITS) - num_sbi_pages + i;
		sbi_pt[idx] = pte_create(sbiAddr >> RISCV_PGSHIFT,
					 PTE_R|PTE_X, 0);
		sbiAddr += RISCV_PGSIZE;
	}
	pte_t* sbi_pte = middle_pt + ((num_middle_pts << RISCV_PGLEVEL_BITS)-1);
	*sbi_pte = ptd_create((uintptr_t)sbi_pt >> RISCV_PGSHIFT);

	// IO space.
	root_pt[0] = pte_create(0, PTE_W|PTE_R, 0);
	root_pt[1] = pte_create(0x40000000>>RISCV_PGSHIFT,
				PTE_W|PTE_R, 0);

	// Start of RAM
	root_pt[2] = pte_create(0x80000000>>RISCV_PGSHIFT,
				PTE_W|PTE_R, 0);
	mb();
	root_page_table = root_pt;
	uintptr_t ptbr = ((uintptr_t) root_pt) >> RISCV_PGSHIFT;
	write_csr(sptbr, ptbr);
}

void initVirtualMemory(void) {
	uintptr_t ms;

	ms = read_csr(mstatus);
	ms = INSERT_FIELD(ms, MSTATUS_VM, VM_CHOICE);
	write_csr(mstatus, ms);
	ms = read_csr(mstatus);

	if (EXTRACT_FIELD(ms, MSTATUS_VM) != VM_CHOICE) {
		printk(BIOS_DEBUG, "We don't have virtual memory...\n");
		return;
	} else {
		printk(BIOS_DEBUG, "-----------------------------\n");
		printk(BIOS_DEBUG, "Virtual memory status enabled\n");
		printk(BIOS_DEBUG, "-----------------------------\n");
	}

	// TODO: Figure out how to grab this from cbfs
	printk(BIOS_DEBUG, "Initializing virtual memory...\n");
	uintptr_t physicalStart = 0x81000000;
	uintptr_t virtualStart = 0xffffffff81000000;
	init_vm(virtualStart, physicalStart, (pte_t *)_pagetables);
	mb();
	flush_tlb();

#if IS_ENABLED(CONFIG_DEBUG_PRINT_PAGE_TABLES)
	printk(BIOS_DEBUG, "Finished initializing virtual memory, starting walk...\n");
	print_page_table();
#else
	printk(BIOS_DEBUG, "Finished initializing virtual memory\n");
#endif
}

void mstatus_init(void)
{
	uintptr_t ms = 0;
	ms = INSERT_FIELD(ms, MSTATUS_FS, 3);
	ms = INSERT_FIELD(ms, MSTATUS_XS, 3);
	write_csr(mstatus, ms);

	clear_csr(mip, MIP_MSIP);
	set_csr(mie, MIP_MSIP);

	/* Configure which exception causes are delegated to supervisor mode */
	set_csr(medeleg,  (1 << CAUSE_MISALIGNED_FETCH)
			| (1 << CAUSE_FAULT_FETCH)
			| (1 << CAUSE_ILLEGAL_INSTRUCTION)
			| (1 << CAUSE_BREAKPOINT)
			| (1 << CAUSE_FAULT_LOAD)
			| (1 << CAUSE_FAULT_STORE)
			| (1 << CAUSE_USER_ECALL)
	);


	/* Enable all user/supervisor-mode counters */
	/* We'll turn these on once lowrisc gets their bitstream up to
	 * 1.9. Right now there's no agreement on the values for these
	 * architectural registers.
	 */
	//write_csr(mscounteren, 0b111);
	//write_csr(mucounteren, 0b111);
}
