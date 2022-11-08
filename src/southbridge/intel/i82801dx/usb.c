/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801dx.h"

static void usb_init(struct device *dev)
{
	printk(BIOS_DEBUG, "USB: Setting up controller.. ");
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
			   PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);
	printk(BIOS_DEBUG, "done.\n");
}

static struct device_operations usb_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init,
	.enable = i82801dx_enable,
};

/* 82801DB/DBL/DBM USB1 */
static const struct pci_driver usb_driver_1 __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82801DB_USB1,
};

/* 82801DB/DBL/DBM USB2 */
static const struct pci_driver usb_driver_2 __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82801DB_USB2,
};

/* 82801DB/DBL/DBM USB3 */
static const struct pci_driver usb_driver_3 __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82801DB_USB3,
};
