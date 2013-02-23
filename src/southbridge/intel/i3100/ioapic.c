/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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


static void read_resources(struct device *dev)
{
	u32 tmp;

	/* Enable IO(X)APIC config space */
	tmp = pci_read_config16(dev, 0x40);
	pci_write_config16(dev, 0x40, tmp & ~(1 << 13));
	/* Enable I/O APIC space at 0xfec80000 */
	dev->path.pci.devfn |= 1;
	pci_write_config16(dev, 0x04, 0x06);
	pci_write_config32(dev, 0x10, 0xfec80000);
	dev->path.pci.devfn &= ~1;
	pci_write_config16(dev, 0x40, tmp);

	pci_bus_read_resources(dev);
}

static struct device_operations pci_ops  = {
	.read_resources   = read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.scan_bus         = pci_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = 0,

};

static const struct pci_driver pci_driver0 __pci_driver = {
	.ops    = &pci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x3500,
};

