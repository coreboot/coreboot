/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Rudolf Marek <r.marek@assembler.cz>
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
#include <device/pci_ops.h>
#include <lib.h>

static void iommu_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, 0x44);
	res->size = 512 * 1024;
	res->align = log2(res->size);
	res->gran = log2(res->size);
	res->limit = 0xffffffff;	/* 4G */
	res->flags = IORESOURCE_MEM;
}

static void iommu_set_resources(struct device *dev)
{
	struct resource *res;

	pci_dev_set_resources(dev);

	res = find_resource(dev, 0x44);
	/* Remember this resource has been stored */
	res->flags |= IORESOURCE_STORED;
	/* For now, do only 32-bit space allocation */
	pci_write_config32(dev, 0x48, 0x0);
	pci_write_config32(dev, 0x44, res->base | (1 << 0));
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations iommu_ops = {
	.read_resources = iommu_read_resources,
	.set_resources = iommu_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = 0,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver iommu_driver __pci_driver = {
	.ops = &iommu_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_15H_MODEL_303F_NB_IOMMU,
};
