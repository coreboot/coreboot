/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <drivers/intel/dptf/chip.h>
#include <fw_config.h>
#include <soc/bootblock.h>
#include <sar.h>
#include <static.h>

WEAK_DEV_PTR(dptf_policy);

static void update_oem_variables(void)
{
	const struct device *policy_dev = DEV_PTR(dptf_policy);
	struct drivers_intel_dptf_config *config = policy_dev->chip_info;

	if (fw_config_probe(FW_CONFIG(THERMAL, THERMAL_FANLESS))) {
		config->oem_data.oem_variables[0] = 0;
		config->oem_data.oem_variables[1] = 1;
		printk(BIOS_INFO, "Set OEM Variable [0] to 0.\n");
		printk(BIOS_INFO, "Set OEM Variable [1] to 1.\n");
	}

	if (fw_config_probe(FW_CONFIG(THERMAL, THERMAL_FAN))) {
		config->oem_data.oem_variables[0] = 1;
		config->oem_data.oem_variables[1] = 0;
		printk(BIOS_INFO, "Set OEM Variable [0] to 1.\n");
		printk(BIOS_INFO, "Set OEM Variable [1] to 0.\n");
	}
}

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_SAR_ID));
}

void variant_devtree_update(void)
{
	struct device *emmc = DEV_PTR(emmc);
	struct device *ufs = DEV_PTR(ufs);
	struct device *ish = DEV_PTR(ish);

	update_oem_variables();

	if (!fw_config_is_provisioned()) {
		printk(BIOS_INFO, "fw_config unprovisioned so enable all storage devices\n");
		return;
	}

	if (!fw_config_probe(FW_CONFIG(STORAGE, STORAGE_EMMC))) {
		printk(BIOS_INFO, "eMMC disabled by fw_config\n");
		emmc->enabled = 0;
	}

	if (!fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UFS))) {
		printk(BIOS_INFO, "UFS disabled by fw_config\n");
		ufs->enabled = 0;
		ish->enabled = 0;
	}
}
