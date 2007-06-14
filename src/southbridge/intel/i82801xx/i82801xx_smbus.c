/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2005 Yinghai Lu <yinghailu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <smbus.h>
#include <pci.h>
#include <arch/io.h>
#include "i82801_model_specific.h"
#include "i82801xx.h"
#include "i82801_smbus.h"

static int smbus_read_byte(struct bus *bus, device_t dev, uint8_t address)
{
	unsigned device;
	struct resource *res;

	device = dev->path.u.i2c.device;
	res = find_resource(bus->dev, 0x20);
	
	return do_smbus_read_byte(res->base, device, address);
}

static struct smbus_bus_operations lops_smbus_bus = {
	.read_byte  = smbus_read_byte,
};

static struct device_operations smbus_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = 0,
	.scan_bus         = scan_static_bus,
	.enable           = i82801er_enable,
	.ops_smbus_bus    = &lops_smbus_bus,
};

/* i82801aa */
static struct pci_driver smbus_driver __pci_driver = {
	.ops    = &smbus_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x2413,
};

/* i82801ab */
static struct pci_driver smbus_driver __pci_driver = {
	.ops    = &smbus_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x2423,
};

/* i82801ba */
static struct pci_driver smbus_driver __pci_driver = {
	.ops    = &smbus_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x2443,
};

/* i82801ca */
static struct pci_driver smbus_driver __pci_driver = {
	.ops    = &smbus_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x2483,
};

/* i82801db and i82801dbm */
static struct pci_driver smbus_driver __pci_driver = {
	.ops    = &smbus_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x24c3,
};

/* i82801eb and i82801er */
static struct pci_driver smbus_driver __pci_driver = {
	.ops    = &smbus_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x24d3,
};

