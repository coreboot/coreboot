#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "cs5535.h"

static void ide_init(struct device *dev)
{
	printk(BIOS_SPEW, "cs5535_ide: %s\n", __func__);
}

static void ide_enable(struct device *dev)
{
	printk(BIOS_SPEW, "cs5535_ide: %s\n", __func__);
}

static struct device_operations ide_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ide_init,
	.enable           = ide_enable,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops 	= &ide_ops,
	.vendor = PCI_VENDOR_ID_NS,
	.device = PCI_DEVICE_ID_NS_CS5535_IDE,
};
