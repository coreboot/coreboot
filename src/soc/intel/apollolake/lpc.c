/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/pci_ids.h>

static void soc_lpc_add_io_resources(device_t dev)
{
	struct resource *res;

	/* Add the default claimed legacy IO range for the LPC device. */
	res = new_resource(dev, 0);
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void soc_lpc_read_resources(device_t dev)
{
	/* Get the PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add IO resources to LPC. */
	soc_lpc_add_io_resources(dev);
}

static struct device_operations device_ops = {
	.read_resources = &soc_lpc_read_resources,
	.set_resources = &pci_dev_set_resources,
	.enable_resources = &pci_dev_enable_resources,
	.write_acpi_tables = southbridge_write_acpi_tables,
};

static const struct pci_driver soc_lpc __pci_driver = {
	.ops = &device_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_APOLLOLAKE_LPC,
};
