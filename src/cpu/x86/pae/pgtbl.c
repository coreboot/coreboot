/*
	2005.12 yhlu add coreboot_ram cross the vga font buffer handling
*/

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/pae.h>
#include <string.h>

static void paging_off(void)
{
	__asm__ __volatile__ (
		/* Disable paging */
		"movl	%%cr0, %%eax\n\t"
		"andl	$0x7FFFFFFF, %%eax\n\t"
		"movl	%%eax, %%cr0\n\t"
		/* Disable pae */
		"movl	%%cr4, %%eax\n\t"
		"andl	$0xFFFFFFDF, %%eax\n\t"
		"movl	%%eax, %%cr4\n\t"
		:
		:
		: "eax"
		);
}

static void paging_on(void *pdp)
{
	__asm__ __volatile__(
		/* Load the page table address */
		"movl	%0, %%cr3\n\t"
		/* Enable pae */
		"movl	%%cr4, %%eax\n\t"
		"orl	$0x00000020, %%eax\n\t"
		"movl	%%eax, %%cr4\n\t"
		/* Enable paging */
		"movl	%%cr0, %%eax\n\t"
		"orl	$0x80000000, %%eax\n\t"
		"movl	%%eax, %%cr0\n\t"
		:
		: "r" (pdp)
		: "eax"
		);
}

void *map_2M_page(unsigned long page)
{
	struct pde {
		uint32_t addr_lo;
		uint32_t addr_hi;
	} __attribute__ ((packed));
	struct pg_table {
		struct pde pd[2048];
		struct pde pdp[512];
	} __attribute__ ((packed));

	static struct pg_table pgtbl[CONFIG_MAX_CPUS] __attribute__ ((aligned(4096)));
	static unsigned long mapped_window[CONFIG_MAX_CPUS];
	unsigned long index;
	unsigned long window;
	void *result;
	int i;
	index = cpu_index();
	if ((index < 0) || (index >= CONFIG_MAX_CPUS)) {
		return MAPPING_ERROR;
	}
	window = page >> 10;
	if (window != mapped_window[index]) {
		paging_off();
		if (window > 1) {
			struct pde *pd, *pdp;
			/* Point the page directory pointers at the page directories */
			memset(&pgtbl[index].pdp, 0, sizeof(pgtbl[index].pdp));
			pd = pgtbl[index].pd;
			pdp = pgtbl[index].pdp;
			pdp[0].addr_lo = ((uint32_t)&pd[512*0])|1;
			pdp[1].addr_lo = ((uint32_t)&pd[512*1])|1;
			pdp[2].addr_lo = ((uint32_t)&pd[512*2])|1;
			pdp[3].addr_lo = ((uint32_t)&pd[512*3])|1;
			/* The first half of the page table is identity mapped */
			for(i = 0; i < 1024; i++) {
				pd[i].addr_lo = ((i & 0x3ff) << 21)| 0xE3;
				pd[i].addr_hi = 0;
			}
			/* The second half of the page table holds the mapped page */
			for(i = 1024; i < 2048; i++) {
				pd[i].addr_lo = ((window & 1) << 31) | ((i & 0x3ff) << 21) | 0xE3;
				pd[i].addr_hi = (window >> 1);
			}
			paging_on(pdp);
		}
		mapped_window[index] = window;
	}
	if (window == 0) {
		result = (void *)(page << 21);
	} else {
		result = (void *)(0x80000000 | ((page & 0x3ff) << 21));
	}
	return result;
}
