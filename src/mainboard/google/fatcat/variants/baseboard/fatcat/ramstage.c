/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

/*
 * SKU_ID, TDP (Watts), pl1_min (milliWatts), pl1_max (milliWatts),
 * pl2_min (milliWatts), pl2_max (milliWatts), pl4 (milliWatts)
 */
const struct cpu_tdp_power_limits power_optimized_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_PTL_H_ID_1,
		.cpu_tdp = TDP_25W,
		.power_limits_index = PTL_H_1_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 25000,
		.pl2_min_power = 50000,
		.pl2_max_power = 50000,
		.pl4_power = 50000 /* TODO: needs fine tuning */
	},
	{
		.mch_id = PCI_DID_INTEL_PTL_H_ID_2,
		.cpu_tdp = TDP_25W,
		.power_limits_index = PTL_H_1_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 25000,
		.pl2_min_power = 50000,
		.pl2_max_power = 50000,
		.pl4_power = 50000 /* TODO: needs fine tuning */
	},
	{
		.mch_id = PCI_DID_INTEL_PTL_H_ID_3,
		.cpu_tdp = TDP_25W,
		.power_limits_index = PTL_H_2_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 25000,
		.pl2_min_power = 50000,
		.pl2_max_power = 50000,
		.pl4_power = 50000 /* TODO: needs fine tuning */
	},
	{
		.mch_id = PCI_DID_INTEL_PTL_H_ID_4,
		.cpu_tdp = TDP_25W,
		.power_limits_index = PTL_H_2_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 25000,
		.pl2_min_power = 50000,
		.pl2_max_power = 50000,
		.pl4_power = 50000 /* TODO: needs fine tuning */
	},
};

void baseboard_devtree_update(void)
{
	/* Don't optimize the power limit if booting with barrel attached */
	if (google_chromeec_is_barrel_charger_present())
		return;

	if (!google_chromeec_is_battery_present())
		variant_update_cpu_power_limits(power_optimized_limits,
						ARRAY_SIZE(power_optimized_limits));
}
