/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <lib.h>

static void iommu_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* IOMMU MMIO registers */
	res = new_resource(dev, 0x44);
	res->size = 512 * 1024;
	res->align = log2(res->size);
	res->gran = log2(res->size);
	res->limit = 0xffffffff;	/* 4G */
	res->flags = IORESOURCE_MEM;
}

static struct device_operations iommu_ops = {
	.read_resources = iommu_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.ops_pci = &pci_dev_ops_pci,
};

static const struct pci_driver iommu_driver __pci_driver = {
	.ops = &iommu_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_16H_MODEL_303F_NB_IOMMU,
};
