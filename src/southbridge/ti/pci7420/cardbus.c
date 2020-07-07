/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/cardbus.h>
#include "pci7420.h"
#include "chip.h"

static void pci7420_cardbus_init(struct device *dev)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;

	struct southbridge_ti_pci7420_config *config = dev->chip_info;
	int smartcard_enabled = 0;

	printk(BIOS_DEBUG, "TI PCI7420/7620 init\n");

	if (!config) {
		printk(BIOS_DEBUG, "PCI7420: No configuration found.\n");
	} else {
		smartcard_enabled = config->smartcard_enabled;
	}

	reg32 = pci_read_config32(dev, SYSCTL);
	reg32 |= RIMUX;
	pci_write_config32(dev, SYSCTL, reg32);

	/* Enable SPKROUT */
	reg8 = pci_read_config8(dev, CARDCTL);
	reg8 |= SPKROUTEN;
	pci_write_config8(dev, CARDCTL, reg8);

	/* Power switch select and FM disable */
	reg16 = pci_read_config16(dev, GENCTL);
	reg16 |= P12V_SW_SEL; // 12V capable power switch
	if (smartcard_enabled == 0)
		reg16 |= DISABLE_FM;
	pci_write_config16(dev, GENCTL, reg16);

	/* Multifunction routing status */
	pci_write_config32(dev, MFUNC, 0x018a1b22);
}

static void pci7420_cardbus_read_resources(struct device *dev)
{
	cardbus_read_resources(dev);
}

static void pci7420_cardbus_set_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "%s In set resources\n",dev_path(dev));

	pci_dev_set_resources(dev);

	printk(BIOS_DEBUG, "%s done set resources\n",dev_path(dev));
}

static struct device_operations ti_pci7420_ops = {
	.read_resources   = pci7420_cardbus_read_resources,
	.set_resources    = pci7420_cardbus_set_resources,
	.enable_resources = cardbus_enable_resources,
	.init             = pci7420_cardbus_init,
	.scan_bus         = pci_scan_bridge,
};

static const struct pci_driver ti_pci7420_driver __pci_driver = {
	.ops    = &ti_pci7420_ops,
	.vendor = 0x104c,
	.device = 0xac8e,
};

static const struct pci_driver ti_pci7620_driver __pci_driver = {
	.ops    = &ti_pci7420_ops,
	.vendor = 0x104c,
	.device = 0xac8d,
};

struct chip_operations southbridge_ti_pci7420_ops = {
	CHIP_NAME("Texas Instruments PCI7420/7620 Cardbus Controller")
};
