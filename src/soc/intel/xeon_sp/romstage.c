/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <intelblocks/rtc.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/romstage.h>
#include <soc/util.h>

void mainboard_romstage_entry(void)
{
	rtc_init();
	if (soc_get_rtc_failed())
		mainboard_rtc_failed();

	fsp_memory_init(false);
	printk(BIOS_DEBUG, "coreboot fsp_memory_init finished...\n");

	unlock_pam_regions();

	save_dimm_info();
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_SPEW, "WARNING: using default FSP-M parameters!\n");
}

__weak void mainboard_rtc_failed(void)
{

}
__weak void save_dimm_info(void) { }
