/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <intelblocks/rtc.h>
#include <console/console.h>
#include <fsp/util.h>
#include <smbios.h>
#include <soc/ddr.h>
#include <soc/intel/common/smbios.h>
#include <soc/romstage.h>
#include <soc/util.h>
#include <spd.h>

void mainboard_romstage_entry(void)
{
	rtc_init();
	if (soc_get_rtc_failed())
		mainboard_rtc_failed();

	fsp_memory_init(false);
	printk(BIOS_DEBUG, "coreboot fsp_memory_init finished...\n");
	mainboard_ewl_check();

	if (CONFIG(ENABLE_FSP_ERROR_INFO)) {
		if (fsp_display_error_info()) {
			mainboard_fsp_error_handle();
			die("ERROR: FSP reported an error(s) after running!");
		}
	}

	save_dimm_info();
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_SPEW, "WARNING: using default FSP-M parameters!\n");
}

__weak void mainboard_rtc_failed(void)
{
}

__weak void mainboard_ewl_check(void) { }

/* mainboard can override this function for their own handling, such as writing a BMC SEL. */
__weak void mainboard_fsp_error_handle(void) { }
