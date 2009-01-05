/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <device/smbus.h>
#include <io.h>
#include <statictree.h>
#include <config.h>
#include "mcp55.h"

#include "mcp55_smbus.h"

static int lsmbus_recv_byte(struct device *dev)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(struct device *dev, u8 val)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device *dev, u8 address, u8 val)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_write_byte(res->base, device, address, val);
}
static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte	= lsmbus_recv_byte,
	.send_byte	= lsmbus_send_byte,
	.read_byte	= lsmbus_read_byte,
	.write_byte	= lsmbus_write_byte,
};

#ifdef HAVE_ACPI_TABLES
unsigned pm_base;
#endif

static void mcp55_sm_read_resources(struct device *dev)
{
	unsigned long index;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	for (index = 0x60; index <= 0x68; index+=4) { // We got another 3.
		pci_get_resource(dev, index);
	}
	compact_resources(dev);

}

static void mcp55_sm_init(struct device *dev)
{
#ifdef CONFIG_HAVE_ACPI_TABLES
	struct resource *res;

	res = find_resource(dev, 0x60);

	if (res)
		pm_base = res->base;
#endif
}

struct device_operations mcp55_smbus = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_SM2}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_sm_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = mcp55_sm_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
	.ops_smbus_bus		 = &lops_smbus_bus,
};

