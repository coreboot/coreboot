/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc
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
#include <device/pci_def.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <console/uart.h>
#include <arch/io.h>

static void oxford_oxpcie_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "Initializing Oxford OXPCIe952\n");

	struct resource *res = find_resource(dev, 0x10);
	if (!res) {
		printk(BIOS_WARNING, "OXPCIe952: No UART resource found.\n");
		return;
	}
	void *bar0 = res2mmio(res, 0, 0);

	printk(BIOS_DEBUG, "OXPCIe952: Class=%x Revision ID=%x\n",
			(read32(bar0) >> 8), (read32(bar0) & 0xff));
	printk(BIOS_DEBUG, "OXPCIe952: %d UARTs detected.\n",
			(read32(bar0 + 4) & 3));
	printk(BIOS_DEBUG, "OXPCIe952: UART BAR: 0x%x\n", (u32)res->base);
}


static void oxford_oxpcie_set_resources(struct device *dev)
{
	pci_dev_set_resources(dev);

	/* Re-initialize OXPCIe base address after set_resources */
	u32 mmio_base = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	oxford_remap(mmio_base & ~0xf);
}

static struct device_operations oxford_oxpcie_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = oxford_oxpcie_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = oxford_oxpcie_enable,
	.scan_bus         = 0,
};

static const struct pci_driver oxford_oxpcie_driver __pci_driver = {
	.ops    = &oxford_oxpcie_ops,
	.vendor = 0x1415,
	.device = 0xc158,
};

static const struct pci_driver oxford_oxpcie_driver_2 __pci_driver = {
	.ops    = &oxford_oxpcie_ops,
	.vendor = 0x1415,
	.device = 0xc11b,
};
