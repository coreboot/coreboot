/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
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
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <arch/acpi.h>
#include <northbridge/intel/x4x/chip.h>
#include <northbridge/intel/x4x/x4x.h>

/* Reserve segments A and B:
 *
 * 0xa0000 - 0xbffff: legacy VGA
 */
static const int legacy_hole_base_k = 0xa0000 / 1024;
static const int legacy_hole_size_k = 128;

static void mch_domain_read_resources(device_t dev)
{
	u64 tom, touud;
	u32 tomk, tsegk, tolud, usable_tomk;
	u32 pcie_config_base, pcie_config_size;
	u32 uma_sizek = 0;

	/* 1024KiB TSEG */
	tsegk = 1 << 10;

	pci_domain_read_resources(dev);

	/* Top of Upper Usable DRAM, including remap */
	touud = pci_read_config16(dev, D0F0_TOUUD);
	touud <<= 20;

	/* Top of Lower Usable DRAM */
	tolud = pci_read_config16(dev, D0F0_TOLUD) & 0xfff0;
	tolud <<= 16;

	/* Top of Memory - does not account for any UMA */
	tom = pci_read_config16(dev, D0F0_TOM) & 0x01ff;
	tom <<= 26;

	printk(BIOS_DEBUG, "TOUUD 0x%llx TOLUD 0x%08x TOM 0x%llx\n",
	       touud, tolud, tom);

	tomk = tolud >> 10;

	/* Graphics memory comes next */
	const u16 ggc = pci_read_config16(dev, D0F0_GGC);
	printk(BIOS_DEBUG, "IGD decoded, subtracting ");

	/* Graphics memory */
	const u32 gms_sizek = decode_igd_memory_size((ggc >> 4) & 0xf);
	printk(BIOS_DEBUG, "%uM UMA", gms_sizek >> 10);

	/* GTT Graphics Stolen Memory Size (GGMS) */
	const u32 gsm_sizek = decode_igd_gtt_size((ggc >> 8) & 0xf);
	printk(BIOS_DEBUG, " and %uM GTT\n", gsm_sizek >> 10);

	uma_sizek = gms_sizek + gsm_sizek + tsegk;
	usable_tomk = tomk - uma_sizek;

	printk(BIOS_INFO, "Available memory below 4GB: %uM\n", usable_tomk >> 10);

	/* Report the memory regions */
	ram_resource(dev, 3, 0, legacy_hole_base_k);
	ram_resource(dev, 4, legacy_hole_base_k + legacy_hole_size_k,
		     (usable_tomk - (legacy_hole_base_k + legacy_hole_size_k)));

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

	printk(BIOS_DEBUG, "Adding UMA memory area base=0x%08x "
	       "size=0x%08x\n", usable_tomk << 10, uma_sizek << 10);
	fixed_mem_resource(dev, 6, usable_tomk, uma_sizek, IORESOURCE_RESERVE);

	/* Some strange hole, reserve it */
	//fixed_mem_resource(dev, 7, usable_tomk - (0x02000000 >> 10), 0x02000000 >> 10, IORESOURCE_RESERVE);

	if (decode_pciebar(&pcie_config_base, &pcie_config_size)) {
		printk(BIOS_DEBUG, "Adding PCIe config bar base=0x%08x "
		       "size=0x%x\n", pcie_config_base, pcie_config_size);
		fixed_mem_resource(dev, 7, pcie_config_base >> 10,
			pcie_config_size >> 10, IORESOURCE_RESERVE);
	}
}

static void mch_domain_set_resources(device_t dev)
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

static void mch_domain_init(device_t dev)
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
	.enable_resources = NULL,
	.init             = mch_domain_init,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci_bus	  = pci_bus_default_ops,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
};


static void cpu_bus_init(device_t dev)
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


static void enable_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
#if CONFIG_HAVE_ACPI_RESUME
		switch (pci_read_config32(dev_find_slot(0, PCI_DEVFN(0, 0)), /*D0F0_SKPD*/0xdc)) {
		case SKPAD_NORMAL_BOOT_MAGIC:
			printk(BIOS_DEBUG, "Normal boot.\n");
			acpi_slp_type=0;
			break;
		case SKPAD_ACPI_S3_MAGIC:
			printk(BIOS_DEBUG, "S3 Resume.\n");
			acpi_slp_type=3;
			break;
		default:
			printk(BIOS_DEBUG, "Unknown boot method, assuming normal.\n");
			acpi_slp_type=0;
			break;
		}
#endif
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

static void x4x_init(void *const chip_info)
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

struct chip_operations northbridge_intel_x4x_ops = {
	CHIP_NAME("Intel 4-Series Northbridge")
	.enable_dev = enable_dev,
	.init = x4x_init,
};
