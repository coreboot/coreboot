/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>

#include <drivers/intel/dptf/chip.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, pl1_min, pl1_max, pl2_min, pl2_max */
	/* PL2 values are for performance configuration */
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_7, 3000, 15000,  55000,  55000 },
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_5, 4000, 28000,  64000,  64000 },
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_3, 5000, 45000, 115000, 115000 },
};

void variant_update_power_limits(void)
{
	const struct device_path policy_path[] = {
		{ .type = DEVICE_PATH_PCI, .pci.devfn = SA_DEVFN_DPTF},
		{ .type = DEVICE_PATH_GENERIC, .generic.id = 0}
	};

	const struct device *policy_dev = find_dev_nested_path(pci_root_bus(),
							policy_path, ARRAY_SIZE(policy_path));
	if (!policy_dev)
		return;

	struct drivers_intel_dptf_config *config = policy_dev->chip_info;

	uint16_t mchid = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);

	for (size_t i = 0; i < ARRAY_SIZE(limits); i++) {
		if (mchid == limits[i].mchid) {
			struct dptf_power_limits *settings = &config->controls.power_limits;
			settings->pl1.min_power = limits[i].pl1_min_power;
			settings->pl1.max_power = limits[i].pl1_max_power;
			settings->pl2.min_power = limits[i].pl2_min_power;
			settings->pl2.max_power = limits[i].pl2_max_power;
			printk(BIOS_INFO, "Overriding DPTF power limits PL1 (%u, %u) PL2 (%u, %u)\n",
					limits[i].pl1_min_power,
					limits[i].pl1_max_power,
					limits[i].pl2_min_power,
					limits[i].pl2_max_power);
		}
	}
}

void variant_devtree_update(void)
{
	variant_update_power_limits();
}
