/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/cardbus.h>
#include <console/console.h>
#include "chip.h"

static void ti_pci1x2y_init(struct device *dev)
{

	printk(BIOS_INFO, "Init of Texas Instruments PCI1x2x PCMCIA/CardBus controller\n");
	struct southbridge_ti_pci1x2x_config *conf = dev->chip_info;

	if (conf) {
		/* System control (offset 0x80) */
		pci_write_config32(dev, 0x80, conf->scr);
		/* Multifunction routing */
		pci_write_config32(dev, 0x8C, conf->mrr);
	}
	/* Set the device control register (0x92) accordingly. */
	pci_write_config8(dev, 0x92, pci_read_config8(dev, 0x92) | 0x02);
}

static void ti_pci1x2y_set_subsystem(struct device *dev, unsigned int vendor,
				     unsigned int device)
{
	/*
	 * Enable change sub-vendor ID. Clear the bit 5 to enable to write
	 * to the sub-vendor/device ids at 40 and 42.
	 */
	pci_write_config32(dev, 0x80, pci_read_config32(dev, 0x080) & ~0x10);
	pci_dev_set_subsystem(dev, vendor, device);
	pci_write_config32(dev, 0x80, pci_read_config32(dev, 0x80) | 0x10);
}

static struct pci_operations ti_pci1x2y_pci_ops = {
	.set_subsystem = ti_pci1x2y_set_subsystem,
};

struct device_operations southbridge_ti_pci1x2x_pciops = {
	.read_resources   = cardbus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = cardbus_enable_resources,
	.init             = ti_pci1x2y_init,
	.ops_pci          = &ti_pci1x2y_pci_ops,
};

static const struct pci_driver ti_pci1225_driver __pci_driver = {
	.ops    = &southbridge_ti_pci1x2x_pciops,
	.vendor = PCI_VID_TI,
	.device = PCI_DID_TI_1225,
};

static const struct pci_driver ti_pci1420_driver __pci_driver = {
	.ops    = &southbridge_ti_pci1x2x_pciops,
	.vendor = PCI_VID_TI,
	.device = PCI_DID_TI_1420,
};

static const struct pci_driver ti_pci1510_driver __pci_driver = {
	.ops    = &southbridge_ti_pci1x2x_pciops,
	.vendor = PCI_VID_TI,
	.device = PCI_DID_TI_1510,
};

static const struct pci_driver ti_pci1520_driver __pci_driver = {
	.ops    = &southbridge_ti_pci1x2x_pciops,
	.vendor = PCI_VID_TI,
	.device = PCI_DID_TI_1520,
};

struct chip_operations southbridge_ti_pci1x2x_ops = {
	CHIP_NAME("TI PCI1x2x Cardbus controller")
};
