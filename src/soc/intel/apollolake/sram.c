/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <soc/iomap.h>

static void read_resources(device_t dev)
{
	struct resource *res;
	pci_dev_read_resources(dev);

	res = new_resource(dev, PCI_BASE_ADDRESS_0);
	res->base = PMC_SRAM_BASE_0;
	res->size = PMC_SRAM_SIZE_0;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, PCI_BASE_ADDRESS_2);
	res->base = PMC_SRAM_BASE_1;
	res->size = PMC_SRAM_SIZE_1;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void set_resources(device_t dev)
{
	struct resource *res;
	pci_dev_set_resources(dev);

	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	pci_write_config32(dev, res->index, res->base);
	dev->command |= PCI_COMMAND_MEMORY;
	res->flags |= IORESOURCE_STORED;
	report_resource_stored(dev, res, " SRAM BAR 0");

	res = find_resource(dev, PCI_BASE_ADDRESS_2);
	pci_write_config32(dev, res->index, res->base);
	dev->command |= PCI_COMMAND_MEMORY;
	res->flags |= IORESOURCE_STORED;
	report_resource_stored(dev, res, " SRAM BAR 1");
}

static const struct device_operations device_ops = {
	.read_resources		= read_resources,
	.set_resources		= set_resources,
	.enable_resources	= pci_dev_enable_resources,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_SRAM,
	PCI_DEVICE_ID_INTEL_GLK_SRAM,
	0,
};

static const struct pci_driver pmc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices= pci_device_ids,
};
