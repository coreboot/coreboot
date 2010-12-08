#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801ex.h"

static void ide_init(struct device *dev)
{
	/* Enable IDE devices and timmings */
	pci_write_config16(dev, 0x40, 0x0a307); // IDE0
	pci_write_config16(dev, 0x42, 0x0a307); // IDE1
	pci_write_config8(dev, 0x48, 0x05);
	pci_write_config16(dev, 0x4a, 0x0101);
	pci_write_config16(dev, 0x54, 0x5055);
	printk(BIOS_DEBUG, "IDE Enabled\n");
}

static void i82801ex_ide_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	/* This value is also visible in uchi[0-2] and smbus functions */
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = i82801ex_ide_set_subsystem,
};
static struct device_operations ide_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ide_init,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops    = &ide_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801ER_IDE,
};

