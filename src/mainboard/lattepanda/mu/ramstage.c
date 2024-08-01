/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/gpio_soc_defs.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>
#include <string.h>
#include <drivers/intel/dptf/chip.h>
#include <intelblocks/power_limit.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, PL4 */
	/* PL2 values are for performance configuration */
	{ PCI_DID_INTEL_ADL_N_ID_1, 15, 3000, 15000,  35000,  35000,  83000 },
	{ PCI_DID_INTEL_ADL_N_ID_2,  6, 3000,  6000,  25000,  25000,  78000 },
	{ PCI_DID_INTEL_ADL_N_ID_3,  6, 3000,  6000,  25000,  25000,  78000 },
	{ PCI_DID_INTEL_ADL_N_ID_4,  6, 3000,  6000,  25000,  25000,  78000 },
	{ PCI_DID_INTEL_ADL_N_ID_5,  6, 3000,  6000,  25000,  25000,  78000 },
};

WEAK_DEV_PTR(dptf_policy);
void variant_update_power_limits(void)
{
	const struct device *policy_dev = DEV_PTR(dptf_policy);
	if (!policy_dev)
		return;

	struct drivers_intel_dptf_config *config = config_of(policy_dev);

	uint16_t mchid = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);

	u8 tdp = get_cpu_tdp();

	for (size_t i = 0; i < ARRAY_SIZE(limits); i++) {
		if (mchid == limits[i].mchid && tdp == limits[i].cpu_tdp) {
			struct dptf_power_limits *settings = &config->controls.power_limits;
			config_t *conf = config_of_soc();
			struct soc_power_limits_config *soc_config = conf->power_limits_config;
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

void variant_devtree_update(void)
{
	variant_update_power_limits();
}
