/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/lapic.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <device/pci.h>
#include <cbmem.h>
#include <soc/baytrail.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <arch/acpi.h>

static const int legacy_hole_base_k = 0xa0000 / 1024;
static const int legacy_hole_size_k = 384;

/* Host Memory Map:
 *
 * +--------------------------+ BMBOUND_HI
 * |     Usable DRAM          |
 * +--------------------------+ 4GiB
 * |     PCI Address Space    |
 * +--------------------------+ BMBOUND
 * |     TPM                  |
 * +--------------------------+ IMR2
 * |     TXE                  |
 * +--------------------------+ IMR1
 * |     iGD                  |
 * +--------------------------+
 * |     GTT                  |
 * +--------------------------+ SMMRRH, IRM0
 * |     TSEG                 |
 * +--------------------------+ SMMRRL
 * |     FSP                  |
 * +--------------------------+ SMMRRL - 2MB
 * |     Usable DRAM          |
 * +--------------------------+ FFFFF
 * |     ROM Area             |
 * +--------------------------+ A0000
 * |     Usable DRAM          |
 * +--------------------------+ 0
 *
 * Note that there are really only a few regions that need to enumerated w.r.t.
 * coreboot's resource model:
 *
 * +--------------------------+ BMBOUND_HI
 * |     Cacheable/Usable     |
 * +--------------------------+ 4GiB
 *
 * +--------------------------+ BMBOUND
 * |    Uncacheable/Reserved  |
 * +--------------------------+ SMMRRH
 * |    Cacheable/Reserved    |
 * +--------------------------+ SMMRRL - 2MB
 * |     Cacheable/Usable     |
 * +--------------------------+ 0
 */

/*
 * Get the top of low memory for use by ACPI
 */
uint32_t nc_read_top_of_low_memory(void)
{
	uint32_t fsp_mem_base = 0;
	GetLowMemorySize(&fsp_mem_base);

	return fsp_mem_base;
}

static int get_pcie_bar(u32 *base)
{
	struct device *dev;
	u32 pciexbar_reg;

	*base = 0;

	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!dev)
		return 0;

	pciexbar_reg = iosf_bunit_read(BUNIT_MMCONF_REG);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	*base = pciexbar_reg & ((1 << 31) | (1 << 30) | (1 << 29) |
				(1 << 28));
	return 256;

}

static int add_fixed_resources(struct device *dev, int index)
{
	struct resource *resource;

	resource = new_resource(dev, index++); /* Local APIC */
	resource->base = LAPIC_DEFAULT_BASE;
	resource->size = 0x00001000;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		    IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	mmio_resource(dev, index++, legacy_hole_base_k, legacy_hole_size_k);

	return index;
}

static void mc_add_dram_resources(struct device *dev)
{
	u32 bmbound, bsmmrrl;
	int index = 0;
	uint64_t highmem_size = 0;
	uint32_t fsp_mem_base = 0;

	GetHighMemorySize(&highmem_size);
	fsp_mem_base=(uint32_t)cbmem_top();

	bmbound = iosf_bunit_read(BUNIT_BMBOUND);
	bsmmrrl = iosf_bunit_read(BUNIT_SMRRL) << 20;

	if (bsmmrrl){
		printk(BIOS_DEBUG, "UMA, GTT & SMM memory location: 0x%x\n"
				"UMA, GTT & SMM memory size: %dM\n",
				bsmmrrl, (bmbound - bsmmrrl) >> 20);

		printk(BIOS_DEBUG, "FSP memory location: 0x%x\nFSP memory size: %dM\n",
				fsp_mem_base, (bsmmrrl - fsp_mem_base) >> 20);
	}

	printk(BIOS_INFO, "Available memory below 4GB: 0x%08x (%dM)\n",
			fsp_mem_base, fsp_mem_base >> 20);

	/* Report the memory regions. */
	ram_resource(dev, index++, 0, legacy_hole_base_k);
	ram_resource(dev, index++, legacy_hole_base_k + legacy_hole_size_k,
	     ((fsp_mem_base >> 10) - (legacy_hole_base_k + legacy_hole_size_k)));

	/* Mark SMM & FSP regions reserved */
	mmio_resource(dev, index++, fsp_mem_base >> 10,
			(bmbound - fsp_mem_base) >> 10);

	if (highmem_size) {
		ram_resource(dev, index++, 0x100000000 >> 10, highmem_size >> 10 );
	}
	printk(BIOS_INFO, "Available memory above 4GB: %lluM\n",
			highmem_size >> 20);

	index = add_fixed_resources(dev, index);
}

static void nc_read_resources(struct device *dev)
{
	u32 pcie_config_base;
	int buses;

	/* Call the normal read_resources */
	pci_dev_read_resources(dev);

	/* We use 0xcf as an unused index for our PCIe bar so that we find it again */
	buses = get_pcie_bar(&pcie_config_base);
	if (buses) {
		struct resource *resource = new_resource(dev, 0xcf);
		mmconf_resource_init(resource, pcie_config_base, buses);
	}

	/* Calculate and add DRAM resources. */
	mc_add_dram_resources(dev);
}

static void nc_enable(struct device *dev)
{
	print_fsp_info();
}

static struct device_operations nc_ops = {
	.read_resources   = nc_read_resources,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = NULL,
	.init             = NULL,
	.enable           = &nc_enable,
	.scan_bus         = NULL,
	.ops_pci          = &soc_pci_ops,
};

static const struct pci_driver nc_driver __pci_driver = {
	.ops    = &nc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = SOC_DEVID,
};
