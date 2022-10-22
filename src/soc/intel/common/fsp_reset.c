/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/util.h>
#include <soc/intel/common/reset.h>
#include <stdint.h>

void chipset_handle_reset(uint32_t status)
{
	if (status == CONFIG_FSP_STATUS_GLOBAL_RESET) {
		printk(BIOS_DEBUG, "GLOBAL RESET!\n");
		global_reset();
	}

	printk(BIOS_ERR, "unhandled reset type %x\n", status);
	die("unknown reset type");
}
