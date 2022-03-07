/* SPDX-License-Identifier: GPL-2.0-only */

/* Driver for BayHub Technology LV2 PCI to SD bridge */

#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "lv2.h"

/*
 * This chip has an errata where PCIe config space registers 0x234, 0x248, and
 * 0x24C only support DWORD access, therefore reprogram these in the `finalize`
 * callback.
 */
static void lv2_enable_ltr(struct device *dev)
{
	u16 max_snoop, max_nosnoop;
	if (!pciexp_get_ltr_max_latencies(dev, &max_snoop, &max_nosnoop))
		return;

	const unsigned int ltr_cap = pciexp_find_extended_cap(dev, PCIE_EXT_CAP_LTR_ID);
	if (!ltr_cap)
		return;

	pci_write_config32(dev, ltr_cap + PCI_LTR_MAX_SNOOP, (max_snoop << 16) |  max_nosnoop);
	printk(BIOS_INFO, "%s: Re-programmed LTR max latencies using chip-specific quirk\n",
	       dev_path(dev));
}

static void lv2_enable(struct device *dev)
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
	pci_update_config32(dev, LV2_PCR_HEX_3F4, LV2_L1_SUBSTATE_OPTIMISE_MASK,
			    LV2_L1_SUBSTATE_OPTIMISE);
	pci_update_config32(dev, LV2_PCR_HEX_300, LV2_TUNING_WINDOW_MASK, LV2_TUNING_WINDOW);
	pci_update_config32(dev, LV2_PCR_HEX_304, LV2_DRIVER_STRENGTH_MASK,
			    LV2_DRIVER_STRENGTH);
	pci_update_config32(dev, LV2_PCR_HEX_308, LV2_RESET_DMA_DISABLE_MASK,
			    LV2_RESET_DMA_DISABLE);
	pci_write_config32(dev, LV2_PROTECT, LV2_PROTECT_ON | LV2_PROTECT_LOCK_ON);
	printk(BIOS_INFO, "BayHub LV2: Power-saving enabled\n");
}

static struct device_operations lv2_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.enable			= lv2_enable,
	.final			= lv2_enable_ltr,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_O2_LV2,
	0
};

static const struct pci_driver bayhub_lv2 __pci_driver = {
	.ops		= &lv2_ops,
	.vendor		= PCI_VID_O2,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_generic_bayhub_lv2_ops = {
	CHIP_NAME("BayHub Technology LV2 PCIe to SD bridge")
};
