/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>

void variant_updm_update(FSP_M_CONFIG *mcfg)
{
	printk(BIOS_INFO, "%s UPDM update\n", __func__);
	if (variant_gets_mb_type_config()) {
		mcfg->telemetry_vddcr_vdd_slope_mA = 32453;
		mcfg->telemetry_vddcr_vdd_offset = 168;
		mcfg->telemetry_vddcr_soc_slope_mA = 22644;
		mcfg->telemetry_vddcr_soc_offset = -70;
	}
}
