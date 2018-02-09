/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
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
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <arch/acpi.h>
#include <northbridge/intel/pineview/pineview.h>

/* Reserve everything between A segment and 1MB:
 *
 * 0xa0000 - 0xbffff: legacy VGA
 * 0xc0000 - 0xcffff: VGA OPROM (needed by kernel)
 * 0xe0000 - 0xfffff: SeaBIOS, if used, otherwise DMI
 */
static const int legacy_hole_base_k = 0xa0000 / 1024;

static void add_fixed_resources(struct device *dev, int index)
{
	struct resource *resource;

	resource = new_resource(dev, index++);
	resource->base = (resource_t) 0xfed00000;
	resource->size = (resource_t) 0x00100000;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	mmio_resource(dev, index++, legacy_hole_base_k,
			(0xc0000 >> 10) - legacy_hole_base_k);
	reserved_ram_resource(dev, index++, 0xc0000 >> 10,
			(0x100000 - 0xc0000) >> 10);
}

static void mch_domain_read_resources(struct device *dev)
{
	u64 tom, touud;
	u32 tomk, tolud, tseg_sizek;
	u32 pcie_config_base, pcie_config_size;
	u16 index;
	const u32 top32memk = 4 * (GiB / KiB);

	index = 3;

	pci_domain_read_resources(dev);

	/* Top of Upper Usable DRAM, including remap */
	touud = pci_read_config16(dev, TOUUD);
	touud <<= 20;

	/* Top of Lower Usable DRAM */
	tolud = pci_read_config16(dev, TOLUD) & 0xfff0;
	tolud <<= 16;

	/* Top of Memory - does not account for any UMA */
	tom = pci_read_config16(dev, TOM) & 0x1ff;
	tom <<= 27;

	printk(BIOS_DEBUG, "TOUUD 0x%llx TOLUD 0x%08x TOM 0x%llx ",
	       touud, tolud, tom);

	tomk = tolud >> 10;

	/* Graphics memory */
	const u16 ggc = pci_read_config16(dev, GGC);
	const u32 gms_sizek = decode_igd_memory_size((ggc >> 4) & 0xf);
	printk(BIOS_DEBUG, "%uM UMA", gms_sizek >> 10);
	tomk -= gms_sizek;

	/* GTT Graphics Stolen Memory Size (GGMS) */
	const u32 gsm_sizek = decode_igd_gtt_size((ggc >> 8) & 0xf);
	printk(BIOS_DEBUG, " and %uM GTT\n", gsm_sizek >> 10);
	tomk -= gsm_sizek;

	const u32 tseg_basek = pci_read_config32(dev, TSEG) >> 10;
	const u32 igd_basek = pci_read_config32(dev, GBSM) >> 10;
	const u32 gtt_basek = pci_read_config32(dev, BGSM) >> 10;

	/* Subtract TSEG size */
	tseg_sizek = gtt_basek - tseg_basek;
	tomk -= tseg_sizek;

	/* Report the memory regions */
	ram_resource(dev, index++, 0, 640);
	ram_resource(dev, index++, 768, tomk - 768);
	reserved_ram_resource(dev, index++, tseg_basek, tseg_sizek);
	reserved_ram_resource(dev, index++, gtt_basek, gsm_sizek);
	reserved_ram_resource(dev, index++, igd_basek, gms_sizek);

	/*
	 * If > 4GB installed then memory from TOLUD to 4GB
	 * is remapped above TOM, TOUUD will account for both
	 */
	touud >>= 10; /* Convert to KB */
	if (touud > top32memk) {
		ram_resource(dev, index++, top32memk, touud - top32memk);
		printk(BIOS_INFO, "Available memory above 4GB: %lluM\n",
		       (touud - top32memk) >> 10);
	}

	if (decode_pciebar(&pcie_config_base, &pcie_config_size)) {
		printk(BIOS_DEBUG, "Adding PCIe config bar base=0x%08x "
			"size=0x%x\n", pcie_config_base, pcie_config_size);
		fixed_mem_resource(dev, index++, pcie_config_base >> 10,
			pcie_config_size >> 10, IORESOURCE_RESERVE);
	}

	add_fixed_resources(dev, index);
}

static void mch_domain_set_resources(struct device *dev)
{
	struct resource *res;

	for (res = dev->resource_list; res; res = res->next)
		report_resource_stored(dev, res, "");

	assign_resources(dev->link_list);
}

static void mch_domain_init(struct device *dev)
{
	u32 reg32;

	/* Enable SERR */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_SERR;
	pci_write_config32(dev, PCI_COMMAND, reg32);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = mch_domain_read_resources,
	.set_resources    = mch_domain_set_resources,
	.init             = mch_domain_init,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci_bus	  = pci_bus_default_ops,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
};

static void cpu_bus_init(struct device *dev)
{
	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = cpu_bus_init,
};


static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_intel_pineview_ops = {
	CHIP_NAME("Intel Pineview Northbridge")
	.enable_dev = enable_dev,
};
