/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
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
#include <soc/iomap.h>
#include <soc/pci_ids.h>

/*
 * The ACPI IO BAR (offset 0x20) is not PCI compliant. We've observed cases
 * where the BAR reads back as 0, but the IO window is open. This also means
 * that it will not respond to PCI probing. In the event that probing the BAR
 * fails, we still need to create a resource for it.
 */
static void read_resources(device_t dev)
{
	struct resource *res;
	pci_dev_read_resources(dev);

	res = new_resource(dev, PCI_BASE_ADDRESS_4);
	res->base = ACPI_PMIO_BASE;
	res->size = ACPI_PMIO_SIZE;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static const struct device_operations device_ops = {
	.read_resources		= read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
};

static const struct pci_driver pmc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_APOLLOLAKE_PMC,
};


