/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
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
		.pl1_min_power = 15000,
		.pl1_max_power = 15000,
		.pl2_min_power = 55000,
		.pl2_max_power = 55000,
		.pl4_power = 114000
	},
	{
		.mchid = PCI_DID_INTEL_RPL_P_ID_4,
		.cpu_tdp = 15,
		.pl1_min_power = 15000,
		.pl1_max_power = 15000,
		.pl2_min_power = 55000,
		.pl2_max_power = 55000,
		.pl4_power = 114000
	},
};

void __weak variant_devtree_update(void)
{
	uint32_t board_version = board_id();
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);

	const struct cpu_power_limits *limits = performance_efficient_limits;
	size_t limits_size = ARRAY_SIZE(performance_efficient_limits);

	variant_update_power_limits(limits, limits_size);

	/* Disable I2C bus device for Touchscreen */
	if (board_version >= 1) {
		struct device *i2c1_dev = DEV_PTR(i2c1);

		if (i2c1_dev)
			i2c1_dev->enabled = false;
	}
}
