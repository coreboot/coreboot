/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ehci.h>
#include "hudson.h"

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void usb_init(struct device *dev)
{
}

static struct device_operations usb_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver usb_0_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_USB_18_0,
};

static const struct pci_driver usb_1_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_USB_18_2,
};

static const struct pci_driver usb_4_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_USB_20_5,
};
