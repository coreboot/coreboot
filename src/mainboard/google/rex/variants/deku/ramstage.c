/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <intelblocks/power_limit.h>
#include <static.h>

/*
 * SKU_ID, TDP (Watts), pl1_min (milliWatts), pl1_max (milliWatts),
 * pl2_min (milliWatts), pl2_max (milliWatts), pl4 (milliWatts)
 * Following values are for performance config as per document #640982
 */
const struct cpu_tdp_power_limits variant_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_1,
		.cpu_tdp = 28,
		.power_limits_index = MTL_P_682_482_CORE,
		.pl1_min_power = 19000,
		.pl1_max_power = 28000,
		.pl2_min_power = 64000,
		.pl2_max_power = 64000,
		.pl4_power = 120000
	},
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_3,
		.cpu_tdp = 28,
		.power_limits_index = MTL_P_682_482_CORE,
		.pl1_min_power = 19000,
		.pl1_max_power = 28000,
		.pl2_min_power = 64000,
		.pl2_max_power = 64000,
		.pl4_power = 120000
	},
};

void variant_devtree_update(void)
{
	struct soc_power_limits_config *soc_config;
	struct soc_intel_meteorlake_config *config = config_of_soc();

	soc_config = variant_get_soc_power_limit_config();
	if (soc_config == NULL)
		return;

	if (config->psys_pl2_watts) {
		soc_config->tdp_psyspl2 = config->psys_pl2_watts;
		printk(BIOS_INFO, "Overriding PsysPL2 (%u)\n", soc_config->tdp_psyspl2);
	}

	const struct cpu_tdp_power_limits *limits = variant_limits;
	size_t total_entries = ARRAY_SIZE(variant_limits);
	variant_update_cpu_power_limits(limits, total_entries);
}
