/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/intel/gma/opregion.h>
#include <fw_config.h>

const char *mainboard_vbt_filename(void)
{
	if (fw_config_probe(FW_CONFIG(DB_DISPLAY, DB_HDMI))) {
		printk(BIOS_INFO, "Use vbt-kinox_HDMI.bin\n");
		return "vbt-kinox_HDMI.bin";
	}
	printk(BIOS_INFO, "Use vbt.bin\n");
	return "vbt.bin";
}
