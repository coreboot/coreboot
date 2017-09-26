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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/acpi.h>
#include <intelblocks/lpc_lib.h>
#include <soc/pm.h>

/* Common weak definition, needs to be implemented in each soc LPC driver. */
__attribute__((weak)) void lpc_init(struct device *dev) { /* no-op */ }

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

static void set_child_resources(struct device *dev);

static void loop_resources(struct device *dev)
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
	set_child_resources(dev);
}

/*
 * Loop through all the child devices' resources, and open up windows to the
 * LPC bus, as appropriate.
 */
static void set_child_resources(struct device *dev)
{
	struct bus *link;
	struct device *child;

	for (link = dev->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling)
			loop_resources(child);
	}
}

static void set_resources(device_t dev)
{
	pci_dev_set_resources(dev);

	/* Now open up windows to devices which have declared resources. */
	set_child_resources(dev);
}

static struct device_operations device_ops = {
	.read_resources = soc_lpc_read_resources,
	.set_resources = set_resources,
	.enable_resources = pci_dev_enable_resources,
	.write_acpi_tables = southbridge_write_acpi_tables,
	.acpi_inject_dsdt_generator = southbridge_inject_dsdt,
	.init = lpc_init,
	.scan_bus = scan_lpc_bus,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_LP_SAMPLE,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM,
	PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM,
	PCI_DEVICE_ID_INTEL_KBP_H_C236,
	PCI_DEVICE_ID_INTEL_KBP_H_PREMIUM,
	PCI_DEVICE_ID_INTEL_KBP_H_QM170,
	PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM_HDCP22,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM_HDCP22,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE_HDCP22,
	PCI_DEVICE_ID_INTEL_KBP_LP_SUPER_SKU,
	PCI_DEVICE_ID_INTEL_KBP_LP_U_PREMIUM,
	PCI_DEVICE_ID_INTEL_KBP_LP_Y_PREMIUM,
	PCI_DEVICE_ID_INTEL_APL_LPC,
	PCI_DEVICE_ID_INTEL_GLK_LPC,
	PCI_DEVICE_ID_INTEL_CNL_BASE_U_LPC,
	PCI_DEVICE_ID_INTEL_CNL_U_PREMIUM_LPC,
	PCI_DEVICE_ID_INTEL_CNL_Y_PREMIUM_LPC,
	0
};

static const struct pci_driver soc_lpc __pci_driver = {
	.ops = &device_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
