/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
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
 */

#include <compiler.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/sram.h>
#include <soc/iomap.h>

__weak void soc_sram_init(struct device *dev) { /* no-op */ }

static void sram_read_resources(struct device *dev)
{
	struct resource *res;
	pci_dev_read_resources(dev);

	res = new_resource(dev, PCI_BASE_ADDRESS_0);
	res->base = SRAM_BASE_0;
	res->size = SRAM_SIZE_0;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, PCI_BASE_ADDRESS_2);
	res->base = SRAM_BASE_2;
	res->size = SRAM_SIZE_2;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static const struct device_operations device_ops = {
	.read_resources		= sram_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= soc_sram_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_SRAM,
	PCI_DEVICE_ID_INTEL_GLK_SRAM,
	0,
};

static const struct pci_driver sram __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
