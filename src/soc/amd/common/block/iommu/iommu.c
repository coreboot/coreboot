/*
 * This file is part of the coreboot project.
 *
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

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations iommu_ops = {
	.read_resources = iommu_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.ops_pci = &lops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_15H_MODEL_303F_NB_IOMMU,
	PCI_DEVICE_ID_AMD_15H_MODEL_707F_NB_IOMMU,
	PCI_DEVICE_ID_AMD_17H_MODEL_101F_NB_IOMMU,
	0
};

static const struct pci_driver iommu_driver __pci_driver = {
	.ops = &iommu_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.devices = pci_device_ids,
};
