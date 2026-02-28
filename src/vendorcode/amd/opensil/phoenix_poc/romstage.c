/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include "opensil_console.h"
#include <xSIM-api.h>
#include <xPRF-api.h>

#include "../opensil.h"

uintptr_t opensil_get_low_usable_dram_address(void)
{
	SilDebugSetup(HostDebugService);
	uintptr_t low_usable_dram_addr = xPrfGetLowUsableDramAddress(0);
	printk(BIOS_DEBUG, "xPrfGetLowUsableDramAddress: 0x%lx\n", low_usable_dram_addr);

	return low_usable_dram_addr;
}
