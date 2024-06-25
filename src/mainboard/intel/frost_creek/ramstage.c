/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ramstage.h>

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	/**
	 * Default eMMC DLL configuration.
	 */
	static BL_SCS_SD_DLL frost_creek_emmc_config = {0x00000500, 0x00000910, 0x2a2b292a,
							0x1c1d251c, 0x0001000c, 0x00001818};

	supd->FspsConfig.PcdEMMCDLLConfigPtr = (UINT32)&frost_creek_emmc_config;
	printk(BIOS_DEBUG, "[cb] PcdEMMCDLLConfigPtr: 0x%08x\n",
	       supd->FspsConfig.PcdEMMCDLLConfigPtr);
}
