/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/ramstage.h>

static struct device_operations i2c_dev_ops = {
	.read_resources		= &pci_dev_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.scan_bus		= &scan_smbus,
	.ops_pci		= &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	0x9d60, 0x9d61, 0x9d62, 0x9d63, 0x9d64, 0x9d65, 0
};

static const struct pci_driver pch_i2c __pci_driver = {
	.ops	 = &i2c_dev_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
