/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_ehci.h>
#include "i82801dx.h"

static void usb2_init(struct device *dev)
{
	printk(BIOS_DEBUG, "USB: Setting up controller.. ");
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
			   PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);
	printk(BIOS_DEBUG, "done.\n");
}

static struct device_operations usb2_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb2_init,
	.enable = i82801dx_enable,
};

/* 82801DB/DBM USB 2.0 */
static const struct pci_driver usb2_driver __pci_driver = {
	.ops = &usb2_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82801DB_EHCI,
};
