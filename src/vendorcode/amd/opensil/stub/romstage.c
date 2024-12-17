/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <inttypes.h>

#include "../opensil.h"

uintptr_t opensil_get_low_usable_dram_address(void)
{
	/* Since the stub doesn't have the openSIL function xPrfGetLowUsableDramAddress to
	   call, we just use 0xc0000000 here which should be a usable value in most cases */
	uintptr_t low_usable_dram_addr = 0xc0000000;

	printk(BIOS_NOTICE, "openSIL stub: %s retuns %" PRIxPTR "\n", __func__, low_usable_dram_addr);

	return low_usable_dram_addr;
}
