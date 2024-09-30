/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/power_limit.h>
#include <soc/pci_devs.h>
#include <static.h>

struct soc_power_limits_config *variant_get_soc_power_limit_config(void)
{
	config_t *config = config_of_soc();
	uint16_t mchid = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);
	u8 tdp = get_cpu_tdp();
	size_t i = 0;

	if (mchid == 0xffff)
		return NULL;

	for (i = 0; i < ARRAY_SIZE(cpuid_to_mtl); i++) {
		if (mchid == cpuid_to_mtl[i].cpu_id && tdp == cpuid_to_mtl[i].cpu_tdp) {
			return &config->power_limits_config[cpuid_to_mtl[i].limits];
		}
	}

	if (i == ARRAY_SIZE(cpuid_to_mtl)) {
		printk(BIOS_ERR, "Cannot find correct ovis sku index.\n");
		return NULL;
	}

	return NULL;
}

/*
 * SKU_ID, TDP (Watts), pl1_min (milliWatts), pl1_max (milliWatts),
 * pl2_min (milliWatts), pl2_max (milliWatts), pl4 (milliWatts)
 * Following values are for performance config as per document #640982
 */
const struct cpu_tdp_power_limits limits[] = {
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_1,
		.cpu_tdp = 28,
		.pl1_min_power = 19000,
		.pl1_max_power = 28000,
		.pl2_min_power = 64000,
		.pl2_max_power = 64000,
		.pl4_power = 120000
	},
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_3,
		.cpu_tdp = 28,
		.pl1_min_power = 19000,
		.pl1_max_power = 28000,
		.pl2_min_power = 64000,
		.pl2_max_power = 64000,
		.pl4_power = 120000
	},
};

void __weak variant_devtree_update(void)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);

	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_cpu_power_limits(limits, total_entries);
}

void baseboard_devtree_update(void)
{
	variant_devtree_update();
}
