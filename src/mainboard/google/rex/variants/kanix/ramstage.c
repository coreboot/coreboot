/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <ec/google/chromeec/ec.h>

/*
 * SKU_ID, TDP (Watts), pl1_min (milliWatts), pl1_max (milliWatts),
 * pl2_min (milliWatts), pl2_max (milliWatts), pl4 (milliWatts)
 * Following values are for performance config as per document #640982
 */

const struct cpu_tdp_power_limits variant_perf_efficient_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_2,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 12000,
		.pl1_max_power = 18000,
		.pl2_min_power = 40000,
		.pl2_max_power = 40000,
		.pl4_power = 84000
	},
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_5,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 12000,
		.pl1_max_power = 18000,
		.pl2_min_power = 40000,
		.pl2_max_power = 40000,
		.pl4_power = 84000
	},
};

const struct cpu_tdp_power_limits variant_power_efficient_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_2,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 12000,
		.pl1_max_power = 18000,
		.pl2_min_power = 40000,
		.pl2_max_power = 40000,
		.pl4_power = 47000
	},
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_5,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 12000,
		.pl1_max_power = 18000,
		.pl2_min_power = 40000,
		.pl2_max_power = 40000,
		.pl4_power = 47000
	},
};

void variant_devtree_update(void)
{
	const struct cpu_tdp_power_limits *limits = variant_perf_efficient_limits;
	size_t limits_size = ARRAY_SIZE(variant_perf_efficient_limits);

	/*
	 * If battery is not present or battery level is at or below critical threshold
	 * to boot a platform with the performance efficient configuration, boot with
	 * the power optimized configuration.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC)) {
		if (!google_chromeec_is_battery_present_and_above_critical_threshold()) {
			limits = variant_power_efficient_limits;
			limits_size = ARRAY_SIZE(variant_power_efficient_limits);
		}
	}

	variant_update_cpu_power_limits(limits, limits_size);
}
