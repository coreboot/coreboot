/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "gpio.h"
#include <console/console.h>
#include <soc/romstage.h>
#include <fsp/api.h>
#include <fsp/soc_binding.h>


/*
* Configure GPIO depend on platform
*/
void mainboard_config_gpios(void)
{
	const int num = ARRAY_SIZE(qdnv01_gpio_table);
	const struct dnv_pad_config *table = qdnv01_gpio_table;

	printk(BIOS_INFO, "GPIO table: 0x%lx, entry num: %d!\n",
	       (uintptr_t)table, num);
	gpio_configure_dnv_pads(table, num);
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Disable Memory Down function */
	mupd->FspmConfig.PcdMemoryDown = 0;

	mupd->FspmConfig.PcdMmioSize = 2;

	/* Enable memory preservation through warm resets and Fast Boot by default */
	mupd->FspmConfig.PcdMemoryPreservation = 1;
	mupd->FspmConfig.PcdFastBoot = 1;
	mupd->FspmConfig.PcdSkipMemoryTest = 1;

	/* FSP debug message level */
	mupd->FspmConfig.PcdFspDebugPrintErrorLevel = 1;
}
