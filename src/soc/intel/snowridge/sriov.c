/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pciexp.h>
#include <device/resource.h>
#include <lib.h>

#include "ramstage.h"

void pciexp_vf_read_resources(struct device *dev)
{
	uint32_t sriov_cap_offset, supported_page_size, system_page_size;
	uint16_t num_vfs, index;
	int align;
	struct resource *resource;

	sriov_cap_offset = pciexp_find_extended_cap(dev, PCIE_EXT_CAP_SRIOV_ID, 0);
	if (sriov_cap_offset == 0) {
		printk(BIOS_DEBUG, "No SRIOV capability for %s!\n", dev_path(dev));
		return;
	}

	printk(BIOS_DEBUG, "%s SRIOV capability found at offset 0x%x\n", dev_path(dev),
	       sriov_cap_offset);

	supported_page_size = pci_read_config32(
		dev, sriov_cap_offset + PCIE_EXT_CAP_SRIOV_SUPPORTED_PAGE_SIZE);
	if (supported_page_size == 0) {
		printk(BIOS_DEBUG, "No supported page size for %s!\n", dev_path(dev));
		return;
	}

	system_page_size =
		pci_read_config32(dev, sriov_cap_offset + PCIE_EXT_CAP_SRIOV_SYSTEM_PAGE_SIZE);
	if ((system_page_size & supported_page_size) == 0) {
		printk(BIOS_ERR, "Unsupportted system page size 0x%08x for %s!\n",
		       system_page_size, dev_path(dev));
		return;
	}

	if (popcnt(system_page_size) != 1) {
		printk(BIOS_ERR, "More than 1 bit is set in system page size for %s!\n",
		       dev_path(dev));
		return;
	}

	num_vfs = pci_read_config16(dev, sriov_cap_offset + PCIE_EXT_CAP_SRIOV_TOTAL_VFS);

	/**
	 * If bit `n` is set, the VFs are required to support a page size of 2 ^ (n + 12).
	 */
	align = __ffs(system_page_size) + 12;

	for (index = PCIE_EXT_CAP_SRIOV_VF_BAR0; index <= PCIE_EXT_CAP_SRIOV_VF_BAR5;
	     index += 4) {
		resource = pci_get_resource(dev, sriov_cap_offset + index);
		if (resource->flags == 0)
			continue;

		printk(BIOS_DEBUG, "%s SRIOV BAR at offset 0x%x\n", dev_path(dev),
		       sriov_cap_offset + index);

		/**
		 * Section 9.3.3.13 of PCIe Base Specification 6.2.
		 */
		resource->align = MAX(resource->align, align);
		resource->gran = MAX(resource->gran, align);
		resource->size = 1 << resource->gran;

		resource->size *= num_vfs;

		/**
		 * Allocate above 4G to avoid exhausting MMIO32 space.
		 */
		if (resource->size >= 16 * MiB)
			resource->flags |= IORESOURCE_ABOVE_4G;

		printk(BIOS_DEBUG, "%s SRIOV BAR size 0x%llx, flags 0x%lx\n", dev_path(dev),
		       resource->size, resource->flags);

		if (resource->flags & IORESOURCE_PCI64)
			index += 4;
	}
}

void pciexp_pf_read_resources(struct device *dev)
{
	uint16_t index;
	struct resource *resource;

	printk(BIOS_SPEW, "Reading resource: %s idx %02x\n", dev_path(dev), PCI_BASE_ADDRESS_0);

	for (index = PCI_BASE_ADDRESS_0; index < PCI_BASE_ADDRESS_0 + (6 << 2);) {
		resource = pci_get_resource(dev, index);
		/**
		 * Allocate above 4G to avoid exhausting MMIO32 space.
		 */
		if (resource->size >= 16 * MiB)
			resource->flags |= IORESOURCE_ABOVE_4G;

		index += (resource->flags & IORESOURCE_PCI64) ? 8 : 4;
	}

	pciexp_vf_read_resources(dev);

	compact_resources(dev);
}
