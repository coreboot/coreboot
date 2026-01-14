/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

/*
 * SKU_ID, TDP (Watts), pl1_min (milliWatts), pl1_max (milliWatts),
 * pl2_min (milliWatts), pl2_max (milliWatts), pl4 (milliWatts)
 */
/* Define a macro for the common power limit values for WCL */
#define COMMON_WCL_POWER_LIMITS	\
	.pl1_min_power = 10000,		\
	.pl1_max_power = 15000,		\
	.pl2_min_power = 35000,		\
	.pl2_max_power = 35000,		\
	.pl4_power = 45000

const struct cpu_tdp_power_limits power_optimized_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_WCL_ID_1,
		.cpu_tdp = TDP_15W,
		.power_limits_index = WCL_CORE,
		COMMON_WCL_POWER_LIMITS
	},
	{
		.mch_id = PCI_DID_INTEL_WCL_ID_2,
		.cpu_tdp = TDP_15W,
		.power_limits_index = WCL_CORE,
		COMMON_WCL_POWER_LIMITS
	},
	{
		.mch_id = PCI_DID_INTEL_WCL_ID_3,
		.cpu_tdp = TDP_15W,
		.power_limits_index = WCL_CORE,
		COMMON_WCL_POWER_LIMITS
	},
	{
		.mch_id = PCI_DID_INTEL_WCL_ID_4,
		.cpu_tdp = TDP_15W,
		.power_limits_index = WCL_CORE,
		COMMON_WCL_POWER_LIMITS
	},
	{
		.mch_id = PCI_DID_INTEL_WCL_ID_5,
		.cpu_tdp = TDP_15W,
		.power_limits_index = WCL_CORE,
		COMMON_WCL_POWER_LIMITS
	},
};

/*
 * Placeholder to check if variant has support for barrel jack for powering
 * on the device.
 *
 * Most of the chromebook device is powering on with USB-C hence, unless overridden
 * by some variant, assume barrel jack not present.
 */
__weak bool variant_is_barrel_charger_present(void)
{
	return false;
}

void baseboard_devtree_update(void)
{
	/* Don't optimize the power limit if booting with barrel attached */
	if (variant_is_barrel_charger_present())
		return;

	if (!google_chromeec_is_battery_present()) {
		printk(BIOS_INFO, "Battery not connected, booting with reduced PL values\n");
		variant_update_cpu_power_limits(power_optimized_limits,
						ARRAY_SIZE(power_optimized_limits));
	}
}
