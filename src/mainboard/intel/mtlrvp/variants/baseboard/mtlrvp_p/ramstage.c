/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <device/pci_ids.h>
#include <intelblocks/power_limit.h>

/*
 * SKU_ID, TDP (Watts), pl1_min (milliWatts), pl1_max (milliWatts),
 * pl2_min (milliWatts), pl2_max (milliWatts), pl4 (milliWatts)
 * Following values are for performance config as per document #640982
 */
const struct cpu_tdp_power_limits limits[] = {
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
};

void variant_devtree_update(void)
{
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_cpu_power_limits(limits, total_entries);
}
