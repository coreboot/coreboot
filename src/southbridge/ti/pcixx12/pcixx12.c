/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <device/cardbus.h>

static void pcixx12_init(struct device *dev)
{
	/* cardbus controller function 1 for CF Socket */
	printk(BIOS_DEBUG, "TI PCIxx12 init\n");
}

static void pcixx12_read_resources(struct device *dev)
{
	cardbus_read_resources(dev);
}

static void pcixx12_set_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "%s In set resources\n",dev_path(dev));

	pci_dev_set_resources(dev);

	printk(BIOS_DEBUG, "%s done set resources\n",dev_path(dev));
}

static struct device_operations ti_pcixx12_ops = {
	.read_resources   = pcixx12_read_resources,
	.set_resources    = pcixx12_set_resources,
	.enable_resources = cardbus_enable_resources,
	.init             = pcixx12_init,
	.scan_bus         = pci_scan_bridge,
};

static const struct pci_driver ti_pcixx12_driver __pci_driver = {
	.ops    = &ti_pcixx12_ops,
	.vendor = 0x104c,
	.device = 0x8039,
};
