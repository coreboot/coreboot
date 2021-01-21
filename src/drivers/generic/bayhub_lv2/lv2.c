/* SPDX-License-Identifier: GPL-2.0-only */

/* Driver for BayHub Technology LV2 PCI to SD bridge */

#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "lv2.h"

static void lv2_init(struct device *dev)
{
	struct drivers_generic_bayhub_lv2_config *config = dev->chip_info;
	pci_dev_init(dev);

	if (!config || !config->enable_power_saving)
		return;
	/*
	 * This procedure for enabling power-saving mode is from the
	 * BayHub BIOS Implementation Guideline document.
	 */
	pci_write_config32(dev, LV2_PROTECT, LV2_PROTECT_OFF | LV2_PROTECT_LOCK_OFF);
	pci_or_config32(dev, LV2_PCR_HEX_FC, LV2_PCIE_PHY_P1_ENABLE);
	pci_update_config32(dev, LV2_PCR_HEX_E0, LV2_PCI_PM_L1_TIMER_MASK, LV2_PCI_PM_L1_TIMER);
	pci_update_config32(dev, LV2_PCR_HEX_FC, LV2_ASPM_L1_TIMER_MASK, LV2_ASPM_L1_TIMER);
	pci_or_config32(dev, LV2_PCR_HEX_A8, LV2_LTR_ENABLE);
	pci_write_config32(dev, LV2_PCR_HEX_234, LV2_MAX_LATENCY_SETTING);
	pci_update_config32(dev, LV2_PCR_HEX_248, LV2_L1_SUBSTATE_SETTING_MASK,
			    LV2_L1_SUBSTATE_SETTING);
	pci_update_config32(dev, LV2_PCR_HEX_3F4, LV2_L1_SUBSTATE_OPTIMISE_MASK,
			    LV2_L1_SUBSTATE_OPTIMISE);
	pci_or_config32(dev, LV2_LINK_CTRL, LV2_LINK_CTRL_CLKREQ);
	pci_update_config32(dev, LV2_PCR_HEX_300, LV2_TUNING_WINDOW_MASK, LV2_TUNING_WINDOW);
	pci_update_config32(dev, LV2_PCR_HEX_304, LV2_DRIVER_STRENGTH_MASK,
			    LV2_DRIVER_STRENGTH);
	pci_update_config32(dev, LV2_PCR_HEX_308, LV2_RESET_DMA_DISABLE_MASK,
			    LV2_RESET_DMA_DISABLE);
	pci_update_config32(dev, LV2_LINK_CTRL, LV2_LINK_CTRL_L1_L0_MASK,
			    LV2_LINK_CTRL_L1_ENABLE);
	pci_write_config32(dev, LV2_PROTECT, LV2_PROTECT_ON | LV2_PROTECT_LOCK_ON);
	printk(BIOS_INFO, "BayHub LV2: Power-saving enabled (link_ctrl=%#x)\n",
			pci_read_config32(dev, LV2_LINK_CTRL));
}

static struct device_operations lv2_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.init			= lv2_init,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_O2_LV2,
	0
};

static const struct pci_driver bayhub_lv2 __pci_driver = {
	.ops		= &lv2_ops,
	.vendor		= PCI_VENDOR_ID_O2,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_generic_bayhub_lv2_ops = {
	CHIP_NAME("BayHub Technology LV2 PCIe to SD bridge")
};
