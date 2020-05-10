/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "gpio.h"
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/soc_binding.h>
#include "bmcinfo.h"

void mainboard_config_gpios(void);
void mainboard_memory_init_params(FSPM_UPD *mupd);

/*
* Configure GPIO depend on platform
*/
void mainboard_config_gpios(void)
{
	size_t num;
	const struct pad_config *table;

	printk(BIOS_SPEW, "Board Serial: %s.\n", bmcinfo_serial());
	/* Configure pads prior to SiliconInit() in case there's any
	* dependencies during hardware initialization.
	*/
	table = tagada_gpio_config;
	num = ARRAY_SIZE(tagada_gpio_config);

	if ((!table) || (!num)) {
		printk(BIOS_ERR, "ERROR: No valid GPIO table found!\n");
		return;
	}

	printk(BIOS_INFO, "GPIO table: 0x%x, entry num:  0x%x!\n",
	       (uint32_t)table, (uint32_t)num);
	gpio_configure_pads(table, num);
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	mupd->FspmConfig.PcdFspDebugPrintErrorLevel =
		bmcinfo_fsp_verbosity_level();

	// Enable Rmt and Fast Boot by default, RMT will be run only on first
	// boot or when dimms change
	mupd->FspmConfig.PcdMrcRmtSupport = 1;
	mupd->FspmConfig.PcdFastBoot = 1;
}
