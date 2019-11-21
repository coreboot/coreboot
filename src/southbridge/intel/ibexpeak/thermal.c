/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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
#include "pch.h"
#include <device/mmio.h>

static void thermal_init(struct device *dev)
{
	struct resource *res;
	u8 *base;
	printk(BIOS_DEBUG, "Thermal init start.\n");

	res = find_resource(dev, 0x10);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	write32(base + 4, 0x3a2b);
	write8(base + 0xe, 0x40);
	write16(base + 0x56, 0xffff);
	write16(base + 0x64, 0xffff);
	write16(base + 0x66, 0xffff);
	write16(base + 0x68, 0xfa);

	write8(base + 1, 0xb8);

	printk(BIOS_DEBUG, "Thermal init done.\n");
}

static struct pci_operations pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations thermal_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = thermal_init,
	.scan_bus = 0,
	.ops_pci = &pci_ops,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_IBEXPEAK_THERMAL,
	0
};

static const struct pci_driver pch_thermal __pci_driver = {
	.ops = &thermal_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
