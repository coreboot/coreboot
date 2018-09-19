/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2016-2018 Siemens AG
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

#include <arch/acpi.h>
#include <console/console.h>
#include <cpu/x86/lapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/acpi.h>

static const int legacy_hole_base_k = 0xa0000 / 1024;
static const int legacy_hole_size_k = 384;

static int add_fixed_resources(struct device *dev, int index)
{
	struct resource *resource;
	u32 pcie_config_base, pcie_config_size;
	pcie_config_base = MCFG_BASE_ADDRESS;
	pcie_config_size = MCFG_BASE_SIZE;

	printk(BIOS_DEBUG, "Adding PCIe config bar base=0x%08x "
		   "size=0x%x\n", pcie_config_base, pcie_config_size);
	resource = new_resource(dev, index++);
	resource->base = (resource_t) pcie_config_base;
	resource->size = (resource_t) pcie_config_size;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

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
	u32 fsp_mem_base, fsp_mem_len;
	u32 tseg_base, tseg_length;
	u32 rsv_base, rsv_length;
	u32 tolm;
	int index = 0;
	uint64_t highmem_size = 0;

	fsp_mem_base = GetFspReservedMemory(FspHobListPtr, &fsp_mem_len);
	highmem_size = GetUsableHighMemTop(FspHobListPtr) - 0x100000000L;
	tseg_base    = GetTsegReservedMemory(FspHobListPtr, &tseg_length);
	tolm         = GetPhysicalLowMemTop(FspHobListPtr);

	printk(BIOS_DEBUG, "\n\n");
	printk(BIOS_DEBUG, "fsp_mem_base:             0x%.8x\n", fsp_mem_base);
	printk(BIOS_DEBUG, "fsp_mem_len:              0x%.8x\n", fsp_mem_len);
	printk(BIOS_DEBUG, "tseg_base:                0x%.8x\n", tseg_base);
	printk(BIOS_DEBUG, "tseg_len:                 0x%.8x\n", tseg_length);
	printk(BIOS_DEBUG, "highmem_size:             0x%.8x %.8x\n",
	       (u32)(highmem_size>>32),
	       (u32)(highmem_size&0xffffffff));
	printk(BIOS_DEBUG, "tolm:                     0x%.8x\n", tolm);
	printk(BIOS_DEBUG, "Top of system low memory: 0x%08x\n", tolm);
	printk(BIOS_DEBUG, "FSP memory location:      0x%x\n (size: %dM)\n",
	       fsp_mem_base, fsp_mem_len >> 20);
	printk(BIOS_DEBUG, "tseg:                     0x%08x (size: 0x%.8x)\n",
	       tseg_base, tseg_length);

	/* Report the memory regions. */
	ram_resource(dev, index++, 0, legacy_hole_base_k);
	ram_resource(dev, index++, legacy_hole_base_k + legacy_hole_size_k,
		 ((fsp_mem_base >> 10) - (legacy_hole_base_k + legacy_hole_size_k)));

	/* Mark SMM & FSP regions reserved */
	mmio_resource(dev, index++, tseg_base >> 10, tseg_length >> 10);
	mmio_resource(dev, index++, fsp_mem_base >> 10, fsp_mem_len >> 10);

	/* Reserve MMIO space */
	rsv_base = fsp_mem_base + fsp_mem_len;
	rsv_length = tseg_base - rsv_base;
	if (rsv_length) {
		mmio_resource(dev, index++, rsv_base >> 10,	rsv_length >> 10);
		printk(BIOS_DEBUG, "Reserved MMIO : 0x%08x length 0x%08x\n",
		       rsv_base, rsv_length);
	}

	rsv_base = tseg_base + tseg_length;
	rsv_length = tolm - rsv_base;
	if (rsv_length) {
		mmio_resource(dev, index++, rsv_base >> 10,	rsv_length >> 10);
		printk(BIOS_DEBUG, "Reserved MMIO : 0x%08x length 0x%08x\n",
		       rsv_base, rsv_length);
	}

	if (highmem_size) {
		ram_resource(dev, index++, 0x100000000 >> 10, highmem_size >> 10);
	}
	printk(BIOS_INFO, "Available memory above 4GB: %lluM\n",
	       highmem_size >> 20);

	index = add_fixed_resources(dev, index);
}

static void nc_read_resources(struct device *dev)
{
	/* Call the normal read_resources */
	pci_dev_read_resources(dev);

	/* Calculate and add DRAM resources. */
	mc_add_dram_resources(dev);
}

static void nc_enable(struct device *dev)
{
	print_fsp_info();
}

static struct device_operations nc_ops = {
	.read_resources           = nc_read_resources,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
	.set_resources            = pci_dev_set_resources,
	.enable_resources         = pci_dev_enable_resources,
	.init                     = NULL,
	.enable                   = &nc_enable,
	.scan_bus                 = 0,
	.ops_pci                  = &soc_pci_ops,
};

static const struct pci_driver nc_driver __pci_driver = {
	.ops    = &nc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = SOC_DEVID,
};

static const struct pci_driver nc_driver_es2 __pci_driver = {
	.ops    = &nc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = SOC_DEVID_ES2,
};
