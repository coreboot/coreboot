/* SPDX-License-Identifier: GPL-2.0-only */

#include <cf9_reset.h>
#include <console/console.h>
#include <fsp/util.h>

void chipset_handle_reset(uint32_t status)
{
	/* Do a hard reset if Quark FSP ever requests a reset */
	printk(BIOS_ERR, "Unknown reset type %x\n", status);
	full_reset();
}
