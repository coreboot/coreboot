//2003 Copyright Tyan

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "amd8111.h"

#if 0

static void lpci_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	pci_write_config32(dev, 0x70,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

#endif

static void amd8111_usb2_enable(struct device *dev)
{
	// Due to buggy USB2 we force it to disable.
	dev->enabled = 0;
	amd8111_enable(dev);
	printk(BIOS_DEBUG, "USB2 disabled.\n");
}

static struct device_operations usb2_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.scan_bus         = 0,
	.enable           = amd8111_usb2_enable,
	// .ops_pci          = &lops_pci,
};

static const struct pci_driver usb2_driver __pci_driver = {
	.ops    = &usb2_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_USB2,
};
