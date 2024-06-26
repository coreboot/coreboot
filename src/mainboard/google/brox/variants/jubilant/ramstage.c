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

const struct cpu_power_limits performance_efficient_limits[] = {
	{
		.mchid = PCI_DID_INTEL_RPL_P_ID_3,
		.cpu_tdp = 15,
		.pl1_min_power = 6000,
		.pl1_max_power = 15000,
		.pl2_min_power = 55000,
		.pl2_max_power = 55000,
		.pl4_power = 114000
	},
	{
		.mchid = PCI_DID_INTEL_RPL_P_ID_4,
		.cpu_tdp = 15,
		.pl1_min_power = 6000,
		.pl1_max_power = 15000,
		.pl2_min_power = 55000,
		.pl2_max_power = 55000,
		.pl4_power = 114000
	},
};

const struct system_power_limits sys_limits[] = {
	/* SKU_ID, TDP (Watts), psys_pl2 (Watts) */
	{ PCI_DID_INTEL_RPL_P_ID_3, 15, 60 },
	{ PCI_DID_INTEL_RPL_P_ID_4, 15, 60 },
};

const struct psys_config psys_config = {
	.efficiency = 86,
};

void __weak variant_devtree_update(void)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);

	const struct cpu_power_limits *limits = performance_efficient_limits;
	size_t limits_size = ARRAY_SIZE(performance_efficient_limits);

	variant_update_power_limits(limits, limits_size);
	variant_update_psys_power_limits(limits, sys_limits, limits_size, &psys_config);
}
