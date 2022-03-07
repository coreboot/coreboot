/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ehci.h>
#include "hudson.h"

static void usb_init(struct device *dev)
{
}

static struct device_operations usb_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init,
	.ops_pci = &pci_dev_ops_pci,
};

static const struct pci_driver usb_0_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_USB_18_0,
};

static const struct pci_driver usb_1_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_USB_18_2,
};

static const struct pci_driver usb_4_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_USB_20_5,
};
