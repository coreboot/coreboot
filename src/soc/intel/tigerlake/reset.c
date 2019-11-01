/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Intel Corp.
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
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <fsp/util.h>
#include <soc/intel/common/reset.h>
#include <soc/pci_devs.h>

void do_global_reset(void)
{
	/* Ask CSE to do the global reset */
	if (!send_heci_reset_req_message(GLOBAL_RESET))
		return;

	/* global reset if CSE fail to reset */
	pmc_global_reset_enable(1);
	do_full_reset();
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
