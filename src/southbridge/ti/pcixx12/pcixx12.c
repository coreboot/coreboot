/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
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

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <device/cardbus.h>

static void pcixx12_init(struct device *dev)
{
	/* cardbus controller function 1 for CF Socket */
	printk(BIOS_DEBUG, "TI PCIxx12 init\n");
}

static void pcixx12_read_resources(struct device *dev)
{
	cardbus_read_resources(dev);
}

static void pcixx12_set_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "%s In set resources\n",dev_path(dev));

	pci_dev_set_resources(dev);

	printk(BIOS_DEBUG, "%s done set resources\n",dev_path(dev));
}

static struct device_operations ti_pcixx12_ops = {
	.read_resources   = pcixx12_read_resources,
	.set_resources    = pcixx12_set_resources,
	.enable_resources = cardbus_enable_resources,
	.init             = pcixx12_init,
	.scan_bus         = pci_scan_bridge,
};

static const struct pci_driver ti_pcixx12_driver __pci_driver = {
	.ops    = &ti_pcixx12_ops,
	.vendor = 0x104c,
	.device = 0x8039,
};

static void southbridge_init(struct device *dev)
{
}

struct chip_operations southbridge_ti_pcixx12_ops = {
	CHIP_NAME("Texas Instruments PCIxx12 Cardbus Controller")
	.enable_dev    = southbridge_init,
};
