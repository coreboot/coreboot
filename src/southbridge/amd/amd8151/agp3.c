/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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

static void agp3bridge_init(struct device *dev)
{
	uint8_t byte;

	/* Enable BM, MEM and IO */
	byte = pci_read_config32(dev, 0x04);
	byte |= 0x07;
	pci_write_config8(dev, 0x04, byte);

	return;
}

static struct device_operations agp3bridge_ops  = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init		  = agp3bridge_init,
	.scan_bus	  = pci_scan_bridge,
};

static const struct pci_driver agp3bridge_driver __pci_driver = {
	.ops    = &agp3bridge_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7455, // AGP Bridge
};

static void agp3dev_enable(struct device *dev)
{
	uint32_t value;

	/* AGP enable */
	value = pci_read_config32(dev, 0xa8);
	value |= (3<<8)|2; //AGP 8x
	pci_write_config32(dev, 0xa8, value);

	/* enable BM and MEM */
	value = pci_read_config32(dev, 0x4);
	value |= 6;
	pci_write_config32(dev, 0x4, value);
#if 0
	/* FIXME: should we add agp aperture base and size here ?
	 * or it is done by AGP drivers */
#endif
}

static struct pci_operations pci_ops_pci_dev = {
	.set_subsystem    = pci_dev_set_subsystem,
};

static struct device_operations agp3dev_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init     = 0,
	.scan_bus = 0,
	.enable   = agp3dev_enable,
	.ops_pci  = &pci_ops_pci_dev,
};

static const struct pci_driver agp3dev_driver __pci_driver = {
	.ops    = &agp3dev_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7454, //AGP Device
};
