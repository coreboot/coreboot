/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
#include <cbmem.h>
#include "chip.h"
#include "gm45.h"
#include "arch/acpi.h"

/* Reserve segments A and B:
 *
 * 0xa0000 - 0xbffff: legacy VGA
 */
static const int legacy_hole_base_k = 0xa0000 / 1024;
static const int legacy_hole_size_k = 128;

static int decode_pcie_bar(u32 *const base, u32 *const len)
{
	*base = 0;
	*len = 0;

	struct device *dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!dev)
		return 0;

	const u32 pciexbar_reg = pci_read_config32(dev, D0F0_PCIEXBAR_LO);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: /* 256MB */
		*base = pciexbar_reg & (0x0f << 28);
		*len = 256 * 1024 * 1024;
		return 1;
	case 1: /* 128M */
		*base = pciexbar_reg & (0x1f << 27);
		*len = 128 * 1024 * 1024;
		return 1;
	case 2: /* 64M */
		*base = pciexbar_reg & (0x3f << 26);
		*len = 64 * 1024 * 1024;
		return 1;
	}

	return 0;
}

static void mch_domain_read_resources(struct device *dev)
{
	u64 tom, touud;
	u32 tomk, tolud, uma_sizek = 0;
	u32 pcie_config_base, pcie_config_size;

	/* Total Memory 2GB example:
	 *
	 *  00000000  0000MB-2014MB  2014MB  RAM     (writeback)
	 *  7de00000  2014MB-2016MB     2MB  GFX GTT (uncached)
	 *  7e000000  2016MB-2048MB    32MB  GFX UMA (uncached)
	 *  80000000   2048MB TOLUD
	 *  80000000   2048MB TOM
	 *
	 * Total Memory 4GB example:
	 *
	 *  00000000  0000MB-3038MB  3038MB  RAM     (writeback)
	 *  bde00000  3038MB-3040MB     2MB  GFX GTT (uncached)
	 *  be000000  3040MB-3072MB    32MB  GFX UMA (uncached)
	 *  be000000   3072MB TOLUD
	 * 100000000   4096MB TOM
	 * 100000000  4096MB-5120MB  1024MB  RAM     (writeback)
	 * 140000000   5120MB TOUUD
	 */

	pci_domain_read_resources(dev);

	/* Top of Upper Usable DRAM, including remap */
	touud = pci_read_config16(dev, D0F0_TOUUD);
	touud <<= 20;

	/* Top of Lower Usable DRAM */
	tolud = pci_read_config16(dev, D0F0_TOLUD) & 0xfff0;
	tolud <<= 16;

	/* Top of Memory - does not account for any UMA */
	tom = pci_read_config16(dev, D0F0_TOM) & 0x1ff;
	tom <<= 27;

	printk(BIOS_DEBUG, "TOUUD 0x%llx TOLUD 0x%08x TOM 0x%llx\n",
	       touud, tolud, tom);

	tomk = tolud >> 10;

	/* Graphics memory comes next */
	const u16 ggc = pci_read_config16(dev, D0F0_GGC);
	if (!(ggc & 2)) {
		printk(BIOS_DEBUG, "IGD decoded, subtracting ");

		/* Graphics memory */
		const u32 gms_sizek = decode_igd_memory_size((ggc >> 4) & 0xf);
		printk(BIOS_DEBUG, "%uM UMA, ", gms_sizek >> 10);
		tomk -= gms_sizek;

		/* GTT Graphics Stolen Memory Size (GGMS) */
		const u32 gsm_sizek = decode_igd_gtt_size((ggc >> 8) & 0xf);
		printk(BIOS_DEBUG, "%uM GTT", gsm_sizek >> 10);
		tomk -= gsm_sizek;

		uma_sizek = gms_sizek + gsm_sizek;
	}
	const u8 esmramc = pci_read_config8(dev, D0F0_ESMRAMC);
	const u32 tseg_sizek = decode_tseg_size(esmramc);
	printk(BIOS_DEBUG, " and %uM TSEG\n", tseg_sizek >> 10);
	tomk -= tseg_sizek;
	uma_sizek += tseg_sizek;

	printk(BIOS_INFO, "Available memory below 4GB: %uM\n", tomk >> 10);

	/* Report the memory regions */
	ram_resource(dev, 3, 0, legacy_hole_base_k);
	ram_resource(dev, 4, legacy_hole_base_k + legacy_hole_size_k,
		     (tomk - (legacy_hole_base_k + legacy_hole_size_k)));

	/*
	 * If >= 4GB installed then memory from TOLUD to 4GB
	 * is remapped above TOM, TOUUD will account for both
	 */
	touud >>= 10; /* Convert to KB */
	if (touud > 4096 * 1024) {
		ram_resource(dev, 5, 4096 * 1024, touud - (4096 * 1024));
		printk(BIOS_INFO, "Available memory above 4GB: %lluM\n",
		       (touud >> 10) - 4096);
	}

	printk(BIOS_DEBUG, "Adding UMA memory area base=0x%llx "
	       "size=0x%llx\n", ((u64)tomk) << 10, ((u64)uma_sizek) << 10);
	/* Don't use uma_resource() as our UMA touches the PCI hole. */
	fixed_mem_resource(dev, 6, tomk, uma_sizek, IORESOURCE_RESERVE);

	if (decode_pcie_bar(&pcie_config_base, &pcie_config_size)) {
		printk(BIOS_DEBUG, "Adding PCIe config bar base=0x%08x "
		       "size=0x%x\n", pcie_config_base, pcie_config_size);
		fixed_mem_resource(dev, 7, pcie_config_base >> 10,
			pcie_config_size >> 10, IORESOURCE_RESERVE);
	}
}

static void mch_domain_set_resources(struct device *dev)
{
	struct resource *resource;
	int i;

	for (i = 3; i < 8; ++i) {
		/* Report read resources. */
		resource = probe_resource(dev, i);
		if (resource)
			report_resource_stored(dev, resource, "");
	}

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

static const char *northbridge_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI || dev->bus->secondary != 0)
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0, 0):
		return "MCHC";
	}

	return NULL;
}

static struct device_operations pci_domain_ops = {
	.read_resources   = mch_domain_read_resources,
	.set_resources    = mch_domain_set_resources,
	.enable_resources = NULL,
	.init             = mch_domain_init,
	.scan_bus         = pci_domain_scan_bus,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
	.acpi_name        = northbridge_acpi_name,
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
	.scan_bus         = 0,
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

static void gm45_init(void *const chip_info)
{
	int dev, fn, bit_base;

	struct device *const d0f0 = dev_find_slot(0, 0);

	/* Hide internal functions based on devicetree info. */
	for (dev = 3; dev > 0; --dev) {
		switch (dev) {
		case 3: /* ME */
			fn = 3;
			bit_base = 6;
			break;
		case 2: /* IGD */
			fn = 1;
			bit_base = 3;
			break;
		case 1: /* PEG */
			fn = 0;
			bit_base = 1;
			break;
		}
		for (; fn >= 0; --fn) {
			const struct device *const d =
				dev_find_slot(0, PCI_DEVFN(dev, fn));
			if (!d || d->enabled) continue;
			const u32 deven = pci_read_config32(d0f0, D0F0_DEVEN);
			pci_write_config32(d0f0, D0F0_DEVEN,
					   deven & ~(1 << (bit_base + fn)));
		}
	}

	const u32 deven = pci_read_config32(d0f0, D0F0_DEVEN);
	if (!(deven & (0xf << 6)))
		pci_write_config32(d0f0, D0F0_DEVEN, deven & ~(1 << 14));
}

struct chip_operations northbridge_intel_gm45_ops = {
	CHIP_NAME("Intel GM45 Northbridge")
	.enable_dev = enable_dev,
	.init = gm45_init,
};
