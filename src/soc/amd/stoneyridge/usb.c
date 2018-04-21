/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <compiler.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_ehci.h>
#include <arch/io.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>


static void set_usb_over_current(struct device *dev)
{
	uint16_t map = USB_OC_DISABLE_ALL;

	if (dev->path.pci.devfn == XHCI_DEVFN) {
		if (mainboard_get_xhci_oc_map(&map) == 0) {
			xhci_pm_write32(XHCI_PM_INDIRECT_INDEX,
						XHCI_OVER_CURRENT_CONTROL);
			xhci_pm_write16(XHCI_PM_INDIRECT_DATA, map);
		}
	}

	if (dev->path.pci.devfn == EHCI1_DEVFN) {
		if (mainboard_get_ehci_oc_map(&map) == 0)
			pci_write_config16(dev, EHCI_OVER_CURRENT_CONTROL, map);
	}
}

int __weak mainboard_get_xhci_oc_map(uint16_t *map)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return -1;
}

int __weak mainboard_get_ehci_oc_map(uint16_t *map)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return -1;
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations usb_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = set_usb_over_current,
	.scan_bus = NULL,
	.ops_pci = &lops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_SB900_USB_18_0,
	PCI_DEVICE_ID_AMD_SB900_USB_18_2,
	PCI_DEVICE_ID_AMD_SB900_USB_20_5,
	PCI_DEVICE_ID_AMD_CZ_USB_0,
	PCI_DEVICE_ID_AMD_CZ_USB_1,
	PCI_DEVICE_ID_AMD_CZ_USB3_0,
	0
};

static const struct pci_driver usb_0_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.devices = pci_device_ids,
};
