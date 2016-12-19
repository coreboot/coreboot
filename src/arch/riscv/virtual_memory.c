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

/* Delegate controls which traps are delegated to the payload. If you
 * wish to temporarily disable some or all delegation you can, in a
 * debugger, set it to a different value (e.g. 0 to have all traps go
 * to M-mode). In practice, this variable has been a lifesaver.  It is
 * still not quite determined which delegation might by unallowed by
 * the spec so for now we enumerate and set them all. */
static int delegate = 0
	| (1 << CAUSE_MISALIGNED_FETCH)
	| (1 << CAUSE_FAULT_FETCH)
	| (1 << CAUSE_ILLEGAL_INSTRUCTION)
	| (1 << CAUSE_BREAKPOINT)
	| (1 << CAUSE_FAULT_LOAD)
	| (1 << CAUSE_FAULT_STORE)
	| (1 << CAUSE_USER_ECALL)
	;

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

// The current RISCV *physical* address space is this:
// * 0 - 2 GiB: miscellaneous IO devices
// * 2 GiB - 4 GiB DRAM
// * top 2048 bytes of memory: SBI (which we round out to a 4K page)
// We have determined, also, that if code references a physical address
// not backed by a device, we'll take a fault. In other words, we don't
// need to finely map the memory-mapped devices as we would on an x86.
// We can use GiB mappings for the IO space and we will take a trap
// if we reference hardware that does not exist.
//
// The intent of the RISCV designers is that pages be set up in M mode
// for lower privilege software. They have also told me that they
// expect, unlike other platforms, that next level software use these
// page tables.  Some kernels (Linux) prefer the old fashioned model,
// where kernel starts with an identity (ID) map and sets up page tables as
// it sees fit.  Other kernels (harvey) are fine with using whatever
// firmware sets up.  We need to accommodate both. So, we set up the
// identity map for Linux, but also set up the map for kernels that
// are more willing to conform to the RISCV model.  The map is as
// follows:
//
// ID map: map IO space and all of DRAM 1:1 using 1 GiB PTEs
// I.e. we use 1 GiB PTEs for 4 GiB.
// Linux/BSD uses this mapping just enough to replace it.
//
// The SBI page is the last page in the 64 bit address space.
// map that using the middle_pts shown below.
//
// Top 2G map, including SBI page: map the 2 Gib - 4 GiB of physical
// address space to 0xffffffff_80000000. This will be needed until the
// GNU toolchain can compile code to run at 0xffffffc000000000,
// i.e. the start of Sv39.
//
// Only Harvey/Plan 9 uses this Mapping, and temporarily.  It can
// never be full removed as we need the 4KiB mapping for the SBI page.
//
// standard RISCV map long term: Map IO space, and all of DRAM, to the *lowest*
// possible negative address for this implementation,
// e.g. 0xffffffc000000000 for Sv39 CPUs. For now we can use GiB PTEs.
//
// RISCV map for now: map IO space, and all of DRAM, starting at
// 0xffff_ffc0_0000_0000, i.e. just as for Sv39.
//
// It is our intent on Harvey (and eventually Akaros) that we use
// this map, once the toolchain can correctly support it.
// We have tested this arrangement and it lets us boot harvey to user mode.
void init_vm(uintptr_t virtMemStart, uintptr_t physMemStart, pte_t *sbi_pt)
{
	memset(sbi_pt, 0, RISCV_PGSIZE);
	// need to leave room for sbi page
	// 0xFFF... - 0xFFFFFFFF81000000 - RISCV_PGSIZE
	intptr_t memorySize = 0x7F000000;

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

	// IO space. Identity mapped.
	root_pt[0x000] = pte_create(0x00000000 >> RISCV_PGSHIFT,
				PTE_R | PTE_W, 0);
	root_pt[0x001] = pte_create(0x40000000 >> RISCV_PGSHIFT,
				PTE_R | PTE_W, 0);
	root_pt[0x002] = pte_create(0x80000000 >> RISCV_PGSHIFT,
				PTE_R | PTE_W | PTE_X, 0);
	root_pt[0x003] = pte_create(0xc0000000 >> RISCV_PGSHIFT,
				PTE_R | PTE_W | PTE_X, 0);

	// Negative address space map at 0xffffffc000000000
	root_pt[0x100] = root_pt[0];
	root_pt[0x101] = root_pt[1];
	root_pt[0x102] = root_pt[2];
	root_pt[0x103] = root_pt[3];

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
	// N.B. We used to map physical from 0x81000000,
	// but since kernels need to be able to see the page tables
	// created by firmware, we're going to map from start of RAM.
	// All this is subject to change as we learn more. Much
	// about RISCV is still in flux.
	printk(BIOS_DEBUG, "Initializing virtual memory...\n");
	uintptr_t physicalStart = 0x80000000;
	uintptr_t virtualStart = 0xffffffff80000000;
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

	// clear any pending timer interrupts.
	clear_csr(mip,  MIP_STIP | MIP_SSIP);

	// enable machine and supervisor timer and
	// all other supervisor interrupts.
	set_csr(mie, MIP_MTIP | MIP_STIP | MIP_SSIP);

	// Delegate supervisor timer and other interrupts
	// to supervisor mode.
	set_csr(mideleg,  MIP_STIP | MIP_SSIP);

	set_csr(medeleg, delegate);

	// Enable all user/supervisor-mode counters using
	// v1.9.1 register addresses.
	// They moved from the earlier spec.
	// Until we trust our toolchain use the hardcoded constants.
	// These were in flux and people who get the older toolchain
	// will have difficult-to-debug failures.
	write_csr(/*mucounteren*/0x320, 7);
	write_csr(/*mscounteren*/0x321, 7);
}
