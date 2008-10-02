/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "ck804.h"

static void usb2_init(struct device *dev)
{
	uint32_t dword;
	dword = pci_read_config32(dev, 0xf8);
	dword |= 40;
	pci_write_config32(dev, 0xf8, dword);
}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations usb2_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = usb2_init,
	// .enable        = ck804_enable,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver usb2_driver __pci_driver = {
	.ops    = &usb2_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_USB2,
};
