/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Arista Networks, Inc.
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

#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/intel/fsp_broadwell_de/chip.h>

#define DEVCTL2 0xb8

static void iou_init(struct device *dev)
{
	const config_t *config = dev->chip_info;
	u16 devctl2;

	/* pcie completion timeout
	   EDS Vol 2, Section 7.2.54 */
	devctl2 = pci_read_config16(dev, DEVCTL2);
	devctl2 = (devctl2 & ~0xf) | (config->pcie_compltoval & 0xf);
	pci_write_config16(dev, DEVCTL2, devctl2);
}

static struct device_operations iou_ops = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.scan_bus         = pci_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.init             = iou_init,
};

static const unsigned short iou_device_ids[] = {
	0x6f02, 0x6f08, 0 };

static const struct pci_driver iou_driver __pci_driver = {
	.ops    = &iou_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = iou_device_ids,
};
