/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/power_limit.h>

#define NO_BATTERY_PL4_WATTS_LIMIT	38

void variant_devtree_update(void)
{
	struct soc_power_limits_config *soc_config;
	u32 pl4_watts = NO_BATTERY_PL4_WATTS_LIMIT;

	soc_config = variant_get_soc_power_limit_config();
	if (soc_config == NULL)
		return;

	/*
	 * If battery is not present, reduce power limits to below 45W,
	 * avoid inability to enter the system.
	 * To avoid poor efficiency of the adapter, leave a margin and
	 * set the powerlimit to 38W.
	 */
	if (!google_chromeec_is_battery_present()) {
		/* Adjust PL4 values */
		printk(BIOS_INFO, "previous PL4 value is %d, override PL4 settings to %d watts\n",
		       soc_config->tdp_pl4, pl4_watts);
		soc_config->tdp_pl4 = pl4_watts;
	}
}
