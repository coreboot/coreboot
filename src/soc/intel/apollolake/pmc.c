/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <console/console.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/iomap.h>
#include <soc/pci_ids.h>

static void pmc_init(device_t dev)
{
	printk(BIOS_SPEW, "%s/%s ( %s )\n",
		__FILE__, __func__, dev_name(dev));
}

static void pmc_read_resources(device_t dev)
{
	struct resource *res;

	mmio_resource(dev, PCI_BASE_ADDRESS_0, PMC_BAR0/KiB, 1);
	mmio_resource(dev, PCI_BASE_ADDRESS_2, PMC_BAR1/KiB, 2);

	res = new_resource(dev, PCI_BASE_ADDRESS_4);
	res->base = ACPI_PMIO_BASE;
	res->size = KiB;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static const struct device_operations device_ops = {
	.read_resources		= pmc_read_resources,
	.set_resources		= DEVICE_NOOP,
	.enable_resources	= DEVICE_NOOP,
	.init			= pmc_init,
	.enable			= DEVICE_NOOP,
};

static const struct pci_driver pmc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_APOLLOLAKE_PMC,
};


