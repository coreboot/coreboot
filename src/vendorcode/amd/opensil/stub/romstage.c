/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <inttypes.h>

uintptr_t cbmem_top_chipset(void)
{
	/* Since the stub doesn't have the openSIL function xPrfGetLowUsableDramAddress to
	   call, we just use 0xc0000000 here which should be a usable value in most cases */
	uintptr_t top_mem = 0xc0000000;

	printk(BIOS_NOTICE, "openSIL stub: %s retuns %" PRIxPTR "\n", __func__, top_mem);

	/* The TSEG MSR has an 8M granularity. TSEG also needs to be aligned to its size so
	   account for potentially ill aligned TOP_MEM. */
	if (CONFIG_SMM_TSEG_SIZE) {
		top_mem -= CONFIG_SMM_TSEG_SIZE;
		top_mem = ALIGN_DOWN(top_mem, CONFIG_SMM_TSEG_SIZE);
	}

	return top_mem;
}
