/*
 * This file is part of the coreboot project.
 *
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

#include <cf9_reset.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/pmclib.h>
#include <soc/intel/common/reset.h>
#include <soc/me.h>
#include <soc/pm.h>

static void do_force_global_reset(void)
{
	/*
	 * BIOS should ensure it does a global reset
	 * to reset both host and Intel ME by setting
	 * PCH PMC [B0:D31:F2 register offset 0xAC bit 20]
	 */
	pmc_global_reset_enable(true);

	/* Now BIOS can write 0x06 or 0x0E to 0xCF9 port
	 * to global reset platform */
	do_full_reset();
}

void do_global_reset(void)
{
	if (!send_global_reset()) {
		/* If ME unable to reset platform then
		 * force global reset using PMC CF9GR register*/
		do_force_global_reset();
	}
}

void chipset_handle_reset(uint32_t status)
{
	switch (status) {
	case FSP_STATUS_RESET_REQUIRED_3: /* Global Reset */
		printk(BIOS_DEBUG, "GLOBAL RESET!!\n");
		global_reset();
		break;
	default:
		printk(BIOS_ERR, "unhandled reset type %x\n", status);
		die("unknown reset type");
		break;
	}
}
