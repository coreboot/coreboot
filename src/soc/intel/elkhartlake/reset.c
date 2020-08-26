/* SPDX-License-Identifier: GPL-2.0-only */

#include <cf9_reset.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <soc/intel/common/reset.h>
#include <soc/pci_devs.h>

void do_global_reset(void)
{
	/* Ask CSE to do the global reset */
	if (cse_request_global_reset(GLOBAL_RESET))
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
