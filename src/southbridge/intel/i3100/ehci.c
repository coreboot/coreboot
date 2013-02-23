/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i3100.h"

static void ehci_init(struct device *dev)
{
}

static void ehci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	u8 access_cntl;
	access_cntl = pci_read_config8(dev, 0x80);
	/* Enable writes to protected registers */
	pci_write_config8(dev, 0x80, access_cntl | 1);
	/* Write the subsystem vendor and device id */
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
	/* Restore protection */
	pci_write_config8(dev, 0x80, access_cntl);
}

static struct pci_operations lops_pci = {
	.set_subsystem = &ehci_set_subsystem,
};
static struct device_operations ehci_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ehci_init,
	.scan_bus         = 0,
	.enable           = i3100_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver ehci_driver __pci_driver = {
	.ops    = &ehci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_EHCI,
};

static const struct pci_driver ehci_driver_ep80579 __pci_driver = {
	.ops    = &ehci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_EP80579_EHCI,
};
