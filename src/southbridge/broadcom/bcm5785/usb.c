/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "bcm5785.h"

static void usb_init(struct device *dev)
{
	uint32_t dword;

	dword = pci_read_config32(dev, 0x04);
	dword |= (1<<2)|(1<<1)|(1<<0);
	pci_write_config32(dev, 0x04, dword);

	pci_write_config8(dev, 0x41, 0x00); // Serversworks said

}

static void lpci_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations usb_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = usb_init,
//	.enable           = bcm5785_enable,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver usb_driver __pci_driver = {
	.ops    = &usb_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM5785_USB,
};
