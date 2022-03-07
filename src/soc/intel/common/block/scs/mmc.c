/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <intelblocks/cfg.h>
#include <intelblocks/mmc.h>

static int mmc_write_dll_reg(void *bar, uint32_t reg, uint32_t val)
{
	int ret = 0;
	if (val) {
		write32(bar + reg, val);
		ret = 1;
	}
	return ret;
}

int set_mmc_dll(void *bar)
{
	const struct soc_intel_common_config *common_config;
	const struct mmc_dll_params *dll_params;
	int override = 0;

	common_config = chip_get_common_soc_structure();
	dll_params = &common_config->emmc_dll;

	override |= mmc_write_dll_reg(bar, EMMC_TX_CMD_CNTL_OFFSET,
		dll_params->emmc_tx_cmd_cntl);

	override |= mmc_write_dll_reg(bar, EMMC_TX_DATA_CNTL1_OFFSET,
		dll_params->emmc_tx_data_cntl1);

	override |= mmc_write_dll_reg(bar, EMMC_TX_DATA_CNTL2_OFFSET,
		dll_params->emmc_tx_data_cntl2);

	override |= mmc_write_dll_reg(bar, EMMC_RX_CMD_DATA_CNTL1_OFFSET,
		dll_params->emmc_rx_cmd_data_cntl1);

	override |= mmc_write_dll_reg(bar, EMMC_RX_STROBE_CNTL_OFFSET,
		dll_params->emmc_rx_strobe_cntl);

	override |= mmc_write_dll_reg(bar, EMMC_RX_CMD_DATA_CNTL2_OFFSET,
		dll_params->emmc_rx_cmd_data_cntl2);

	if (override == 0) {
		printk(BIOS_INFO, "Skip Emmc dll value programming\n");
		return -1;
	}

	return 0;
}

static void mmc_soc_init(struct device *dev)
{
	const struct resource *res;

	if (!CONFIG(SOC_INTEL_COMMON_MMC_OVERRIDE))
		return;

	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	set_mmc_dll((void *)(uintptr_t)(res->base));
}

static struct device_operations dev_ops = {
	.read_resources			= pci_dev_read_resources,
	.set_resources			= pci_dev_set_resources,
	.enable_resources		= pci_dev_enable_resources,
	.init				= mmc_soc_init,
	.ops_pci			= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_CMP_EMMC,
	PCI_DID_INTEL_JSP_EMMC,
	PCI_DID_INTEL_ADP_EMMC,
	0
};

static const struct pci_driver pch_sd __pci_driver = {
	.ops		= &dev_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
