/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <fsp/util.h>

void chipset_handle_reset(uint32_t status)
{
	switch (status) {
	case FSP_STATUS_RESET_REQUIRED_5: /* Global Reset */
		die("Global Reset not implemented!\n");
		break;
	default:
		printk(BIOS_ERR, "unhandled reset type %x\n", status);
		die("unknown reset type");
		break;
	}
}
