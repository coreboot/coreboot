/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/iommu.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <lib.h>

static void iommu_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* IOMMU MMIO registers */
	res = new_resource(dev, IOMMU_CAP_BASE_LO);
	res->size = 512 * KiB;
	res->align = log2(res->size);
	res->gran = log2(res->size);
	res->limit = 0xffffffff;	/* 4G */
	res->flags = IORESOURCE_MEM;
}


static void iommu_enable_resources(struct device *dev)
{
	uint32_t base = pci_read_config32(dev, IOMMU_CAP_BASE_LO);
	base |= IOMMU_ENABLE;
	pci_write_config32(dev, IOMMU_CAP_BASE_LO, base);
	printk(BIOS_DEBUG, "%s -> mmio enable: %08X\n", __func__,
	       pci_read_config32(dev, IOMMU_CAP_BASE_LO));
	pci_dev_enable_resources(dev);
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *iommu_acpi_name(const struct device *dev)
{
	return "IOMM";
}
#endif

struct device_operations amd_iommu_ops = {
	.read_resources = iommu_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = iommu_enable_resources,
	.ops_pci = &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = iommu_acpi_name,
#endif
};
