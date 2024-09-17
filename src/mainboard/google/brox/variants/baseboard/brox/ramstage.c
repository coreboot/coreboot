/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <drivers/intel/dptf/chip.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/power_limit.h>
#include <soc/pci_devs.h>


WEAK_DEV_PTR(dptf_policy);

static bool get_sku_index(const struct cpu_power_limits *limits, size_t num_entries,
					size_t *intel_idx, size_t *brox_idx)
{
	uint16_t mchid = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);
	u8 tdp = get_cpu_tdp();
	size_t i = 0;

	for (i = 0; i < ARRAY_SIZE(cpuid_to_adl); i++) {
		if (mchid == cpuid_to_adl[i].cpu_id && tdp == cpuid_to_adl[i].cpu_tdp) {
			*intel_idx = cpuid_to_adl[i].limits;
			break;
		}
	}

	if (i == ARRAY_SIZE(cpuid_to_adl)) {
		printk(BIOS_ERR, "Cannot find correct intel sku index (mchid = %u).\n", mchid);
		return false;
	}

	for (i = 0; i < num_entries; i++) {
		if (mchid == limits[i].mchid && tdp == limits[i].cpu_tdp) {
			*brox_idx = i;
			break;
		}
	}

	if (i == num_entries) {
		printk(BIOS_ERR, "Cannot find brox sku index (mchid = %u)\n", mchid);
		return false;
	}

	return true;
}

static void variant_pl4_override(struct soc_power_limits_config *config,
					const struct cpu_power_limits *limits, size_t brox_idx)
{
	if (!config->tdp_pl4)
		return;

	/* limiting PL4 value for battery disconnected or below critical threshold */
	if (CONFIG_PL4_LIMIT_FOR_CRITICAL_BAT_BOOT &&
	     (!google_chromeec_is_battery_present_and_above_critical_threshold()))
		config->tdp_pl4 = CONFIG_PL4_LIMIT_FOR_CRITICAL_BAT_BOOT;
	else
		config->tdp_pl4 = DIV_ROUND_UP(limits[brox_idx].pl4_power, MILLIWATTS_TO_WATTS);
}

void variant_update_power_limits(const struct cpu_power_limits *limits, size_t num_entries)
{
	const struct device *policy_dev;
	size_t intel_idx, brox_idx;
	struct drivers_intel_dptf_config *config;
	struct dptf_power_limits *settings;
	config_t *conf;
	struct soc_power_limits_config *soc_config;

	if (!num_entries)
		return;

	if (!get_sku_index(limits, num_entries, &intel_idx, &brox_idx))
		return;

	conf = config_of_soc();
	soc_config = &conf->power_limits_config[intel_idx];
	variant_pl4_override(soc_config, limits, brox_idx);

	policy_dev = DEV_PTR(dptf_policy);
	if (!policy_dev) {
		printk(BIOS_INFO, "DPTF policy not set\n");
		return;
	}
	config = policy_dev->chip_info;
	settings = &config->controls.power_limits;
	settings->pl1.min_power = limits[brox_idx].pl1_min_power;
	settings->pl1.max_power = limits[brox_idx].pl1_max_power;
	settings->pl2.min_power = limits[brox_idx].pl2_min_power;
	settings->pl2.max_power = limits[brox_idx].pl2_max_power;

	if (soc_config->tdp_pl2_override != 0) {
		settings->pl2.max_power = soc_config->tdp_pl2_override * MILLIWATTS_TO_WATTS;
		settings->pl2.min_power = settings->pl2.max_power;
	}

	printk(BIOS_INFO, "Overriding power limits PL1 (%u, %u) PL2 (%u, %u) PL4 (%u)\n",
		settings->pl1.min_power, settings->pl1.max_power, settings->pl2.min_power,
		settings->pl2.max_power, soc_config->tdp_pl4);
}
