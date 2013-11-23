/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Vladimir Serbinenko
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "pch.h"
#include <usbdebug.h>
#include <arch/io.h>

static void thermal_init(struct device *dev)
{
	struct resource *res;

	printk(BIOS_DEBUG, "Thermal init start.\n");

	res = find_resource(dev, 0x10);
	if (!res)
		return;

	write32(res->base + 4, 0x3a2b);
	write8(res->base + 0xe, 0x40);
	write32(res->base + 0x12, 0x1a40);
	write16(res->base + 0x16, 0x7746);
	write16(res->base + 0x1a, 0x10f0);
	write16(res->base + 0x56, 0xffff);
	write16(res->base + 0x64, 0xffff);
	write16(res->base + 0x66, 0xffff);
	write16(res->base + 0x68, 0xfa);

	write8(res->base + 1, 0xb8);

	printk(BIOS_DEBUG, "Thermal init done.\n");
}

static void set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   ((device & 0xffff) << 16) | (vendor &
								0xffff));
	}
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations thermal_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = thermal_init,
	.scan_bus = 0,
	.ops_pci = &pci_ops,
};

static const unsigned short pci_device_ids[] = { 0x3b32, 0 };

static const struct pci_driver pch_thermal __pci_driver = {
	.ops = &thermal_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
