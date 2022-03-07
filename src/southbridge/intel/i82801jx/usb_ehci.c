/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "i82801jx.h"
#include <device/pci_ehci.h>

static void usb_ehci_init(struct device *dev)
{
	printk(BIOS_DEBUG, "EHCI: Setting up controller.. ");
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	printk(BIOS_DEBUG, "done.\n");
}

static void usb_ehci_set_subsystem(struct device *dev, unsigned int vendor,
				   unsigned int device)
{
	u8 access_cntl;

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	pci_dev_set_subsystem(dev, vendor, device);

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);
}

static const unsigned short pci_device_ids[] = {
	0x3a3a,
	0x3a6a,
	0x3a3c,
	0x3a6c,
	0
};

static struct pci_operations lops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources		= pci_ehci_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_ehci_init,
	.ops_pci		= &lops_pci,
};

static const struct pci_driver pch_usb_ehci1 __pci_driver = {
	.ops	= &usb_ehci_ops,
	.vendor	= PCI_VID_INTEL,
	.devices = pci_device_ids,
};
