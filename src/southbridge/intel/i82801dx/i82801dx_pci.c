/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Ronald G. Minnich
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801dx.h"

static void pci_init(struct device *dev)
{
	/* Enable pci error detecting */
	uint32_t dword;
	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1 << 8);	/* SERR# Enable */
	dword |= (1 << 6);	/* Parity Error Response */
	pci_write_config32(dev, 0x04, dword);
}

static struct device_operations pci_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = pci_init,
	.scan_bus = pci_scan_bridge,
};

/* 82801DB */
static const struct pci_driver pci_driver_db __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DB_PCI,
};

/* 82801DBM/DBL */
static const struct pci_driver pci_driver_dbm __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DBM_PCI,
};
