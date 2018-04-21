/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corp.
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
#include <intelblocks/acpi.h>
#include <intelblocks/lpc_lib.h>
#include <soc/pm.h>

/* SoC overrides */

/* Common weak definition, needs to be implemented in each soc LPC driver. */
__weak void lpc_soc_init(struct device *dev)
{
	/* no-op */
}

/* Fill up LPC IO resource structure inside SoC directory */
__weak void pch_lpc_soc_fill_io_resources(struct device *dev)
{
	/* no-op */
}

void pch_lpc_add_new_resource(struct device *dev, uint8_t offset,
	uintptr_t base, size_t size, unsigned long flags)
{
	struct resource *res;
	res = new_resource(dev, offset);
	res->base = base;
	res->size = size;
	res->flags = flags;
}

static void pch_lpc_add_io_resources(device_t dev)
{
	/* Add the default claimed legacy IO range for the LPC device. */
	pch_lpc_add_new_resource(dev, 0, 0, 0x1000, IORESOURCE_IO |
			IORESOURCE_ASSIGNED | IORESOURCE_FIXED);

	/* SoC IO resource overrides */
	pch_lpc_soc_fill_io_resources(dev);
}

static void pch_lpc_read_resources(device_t dev)
{
	/* Get the PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add IO resources to LPC. */
	pch_lpc_add_io_resources(dev);
}

static void pch_lpc_set_child_resources(struct device *dev);

static void pch_lpc_loop_resources(struct device *dev)
{
	struct resource *res;

	for (res = dev->resource_list; res; res = res->next) {
		if (res->flags & IORESOURCE_IO)
			lpc_open_pmio_window(res->base, res->size);

		if (res->flags & IORESOURCE_MEM) {
			/* Check if this is already decoded. */
			if (lpc_fits_fixed_mmio_window(res->base, res->size))
				continue;

			lpc_open_mmio_window(res->base, res->size);
		}
	}
	pch_lpc_set_child_resources(dev);
}

/*
 * Loop through all the child devices' resources, and open up windows to the
 * LPC bus, as appropriate.
 */
static void pch_lpc_set_child_resources(struct device *dev)
{
	struct bus *link;
	struct device *child;

	for (link = dev->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling)
			pch_lpc_loop_resources(child);
	}
}

static void pch_lpc_set_resources(device_t dev)
{
	pci_dev_set_resources(dev);

	/* Now open up windows to devices which have declared resources. */
	pch_lpc_set_child_resources(dev);
}

static struct device_operations device_ops = {
	.read_resources			= pch_lpc_read_resources,
	.set_resources			= pch_lpc_set_resources,
	.enable_resources		= pci_dev_enable_resources,
	.write_acpi_tables		= southbridge_write_acpi_tables,
	.acpi_inject_dsdt_generator	= southbridge_inject_dsdt,
	.init				= lpc_soc_init,
	.scan_bus			= scan_lpc_bus,
	.ops_pci			= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_LP_SAMPLE,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM,
	PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM,
	PCI_DEVICE_ID_INTEL_SPT_H_C236,
	PCI_DEVICE_ID_INTEL_SPT_H_PREMIUM,
	PCI_DEVICE_ID_INTEL_SPT_H_QM170,
	PCI_DEVICE_ID_INTEL_KBP_H_Q270,
	PCI_DEVICE_ID_INTEL_KBP_H_H270,
	PCI_DEVICE_ID_INTEL_KBP_H_Z270,
	PCI_DEVICE_ID_INTEL_KBP_H_Q250,
	PCI_DEVICE_ID_INTEL_KBP_H_B250,
	PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM_HDCP22,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM_HDCP22,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE_HDCP22,
	PCI_DEVICE_ID_INTEL_KBP_LP_SUPER_SKU,
	PCI_DEVICE_ID_INTEL_KBP_LP_U_PREMIUM,
	PCI_DEVICE_ID_INTEL_KBP_LP_Y_PREMIUM,
	PCI_DEVICE_ID_INTEL_APL_LPC,
	PCI_DEVICE_ID_INTEL_GLK_LPC,
	PCI_DEVICE_ID_INTEL_GLK_ESPI,
	PCI_DEVICE_ID_INTEL_CNL_BASE_U_LPC,
	PCI_DEVICE_ID_INTEL_CNL_U_PREMIUM_LPC,
	PCI_DEVICE_ID_INTEL_CNL_Y_PREMIUM_LPC,
	0
};

static const struct pci_driver pch_lpc __pci_driver = {
	.ops = &device_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
