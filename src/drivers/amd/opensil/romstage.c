/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <cpu/amd/mtrr.h>
#include <vendorcode/amd/opensil/opensil.h>

uintptr_t cbmem_top_chipset(void)
{
	printk(BIOS_DEBUG, "TOM1: 0x%x\n", get_top_of_mem_below_4gb());
	uintptr_t top_mem = opensil_get_low_usable_dram_address();

	/* The TSEG MSR has an 8M granularity. TSEG also needs to be aligned to its size to
	   account for potentially ill aligned TOP_MEM. */
	if (CONFIG_SMM_TSEG_SIZE) {
		top_mem -= CONFIG_SMM_TSEG_SIZE;
		top_mem = ALIGN_DOWN(top_mem, CONFIG_SMM_TSEG_SIZE);
	}

	return top_mem;
}
