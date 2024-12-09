/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/pci_ids.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/power_limit.h>

/*
 * SKU_ID, TDP (Watts), pl1_min (milliWatts), pl1_max (milliWatts),
 * pl2_min (milliWatts), pl2_max (milliWatts), pl4 (milliWatts)
 * Following values are for performance config as per document #640982
 */

const struct cpu_tdp_power_limits performance_efficient_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_2,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 15000,
		.pl2_min_power = 57000,
		.pl2_max_power = 57000,
		.pl4_power = 114000
	},
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_5,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 15000,
		.pl2_min_power = 57000,
		.pl2_max_power = 57000,
		.pl4_power = 114000
	},
};

const struct cpu_tdp_power_limits power_optimized_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_2,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 15000,
		.pl2_min_power = 57000,
		.pl2_max_power = 57000,
		.pl4_power = 64000
	},
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_5,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 15000,
		.pl2_min_power = 57000,
		.pl2_max_power = 57000,
		.pl4_power = 64000
	},
};

void __weak variant_devtree_update(void)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);

	const struct cpu_tdp_power_limits *limits = performance_efficient_limits;
	size_t limits_size = ARRAY_SIZE(performance_efficient_limits);

	/*
	 * If battery is not present or battery level is at or below critical threshold
	 * to boot a platform with the performance efficient configuration, boot with
	 * the power optimized configuration.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC)) {
		if (!google_chromeec_is_battery_present_and_above_critical_threshold()) {
			limits = power_optimized_limits;
			limits_size = ARRAY_SIZE(power_optimized_limits);
		}
	}

	variant_update_cpu_power_limits(limits, limits_size);
}

void baseboard_devtree_update(void)
{
	variant_devtree_update();
}
