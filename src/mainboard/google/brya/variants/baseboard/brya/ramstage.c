/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <static.h>

#include <drivers/intel/dptf/chip.h>
#include <intelblocks/power_limit.h>

WEAK_DEV_PTR(dptf_policy);

struct soc_power_limits_config *variant_get_soc_power_limit_config(void)
{
	config_t *config = config_of_soc();
	size_t i;
	struct device *sa = pcidev_path_on_root(SA_DEVFN_ROOT);
	uint16_t sa_pci_id;
	u8 tdp;

	if (!sa)
		return NULL;

	sa_pci_id = pci_read_config16(sa, PCI_DEVICE_ID);

	if (sa_pci_id == 0xffff)
		return NULL;

	tdp = get_cpu_tdp();

	for (i = 0; i < ARRAY_SIZE(cpuid_to_adl); i++) {
		if (sa_pci_id == cpuid_to_adl[i].cpu_id &&
				tdp == cpuid_to_adl[i].cpu_tdp) {
			return &config->power_limits_config[cpuid_to_adl[i].limits];
		}
	}

	return NULL;
}

void variant_update_power_limits(const struct cpu_power_limits *limits, size_t num_entries)
{
	if (!num_entries)
		return;

	struct soc_power_limits_config *soc_config = variant_get_soc_power_limit_config();
	if (!soc_config)
		return;

	const struct device *policy_dev = DEV_PTR(dptf_policy);
	if (!policy_dev)
		return;

	struct drivers_intel_dptf_config *config = policy_dev->chip_info;

	uint16_t mchid = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);

	u8 tdp = get_cpu_tdp();

	for (size_t i = 0; i < num_entries; i++) {
		if (mchid == limits[i].mchid && tdp == limits[i].cpu_tdp) {
			struct dptf_power_limits *settings = &config->controls.power_limits;
			settings->pl1.min_power = limits[i].pl1_min_power;
			settings->pl1.max_power = limits[i].pl1_max_power;
			settings->pl2.min_power = limits[i].pl2_min_power;
			settings->pl2.max_power = limits[i].pl2_max_power;
			soc_config->tdp_pl4 = DIV_ROUND_UP(limits[i].pl4_power,
								MILLIWATTS_TO_WATTS);
			printk(BIOS_INFO, "Overriding power limits PL1 (%u, %u) PL2 (%u, %u) PL4 (%u)\n",
				limits[i].pl1_min_power,
				limits[i].pl1_max_power,
				limits[i].pl2_min_power,
				limits[i].pl2_max_power,
				limits[i].pl4_power);
		}
	}
}
