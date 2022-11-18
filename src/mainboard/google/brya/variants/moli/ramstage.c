/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <drivers/intel/gma/opregion.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <intelblocks/power_limit.h>
#include <drivers/intel/dptf/chip.h>
#include <boardid.h>
#include <soc/intel/common/block/pcie/rtd3/chip.h>
#include <acpi/acpi_device.h>

static void devtree_update_emmc_rtd3(void)
{
	uint32_t board_ver = board_id();
	struct device *emmc_rtd3 = DEV_PTR(emmc_rtd3);
	struct soc_intel_common_block_pcie_rtd3_config *config = emmc_rtd3->chip_info;
	if (board_ver <= 1)
		return;

	config->enable_gpio = (struct acpi_gpio)ACPI_GPIO_OUTPUT_ACTIVE_HIGH(GPP_A21);
}

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 28000, 28000,  64000,  64000,  90000 },
};

const struct system_power_limits sys_limits[] = {
	/* SKU_ID, TDP (Watts), psys_pl2 (Watts) */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 135 },
};

static void update_oem_variables(void)
{
	struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	uint16_t mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	const struct device *policy_dev = DEV_PTR(dptf_policy);
	struct drivers_intel_dptf_config *config = policy_dev->chip_info;

	switch (mch_id) {
	case PCI_DID_INTEL_ADL_P_ID_5:
		config->oem_data.oem_variables[0] = 0;
		break;

	case PCI_DID_INTEL_ADL_P_ID_6:
		config->oem_data.oem_variables[0] = 1;
		break;

	case PCI_DID_INTEL_ADL_P_ID_7:
		config->oem_data.oem_variables[0] = 1;
		break;

	case PCI_DID_INTEL_ADL_P_ID_10:
		config->oem_data.oem_variables[0] = 1;
		break;

	default:
		config->oem_data.oem_variables[0] = 1;
	}
}

const struct psys_config psys_config = {
	.efficiency = 97,
	.psys_imax_ma = 11000,
	.bj_volts_mv = 19500
};

void variant_devtree_update(void)
{
	devtree_update_emmc_rtd3();
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_psys_power_limits(limits, sys_limits, total_entries, &psys_config);
	variant_update_power_limits(limits, total_entries);
	update_oem_variables();
}
