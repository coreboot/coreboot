/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2013 Vladimir Serbinenko
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
#include <arch/acpi.h>
#include <arch/io.h>
#include <stdint.h>
#include <delay.h>
#include <cpu/intel/model_2065x/model_2065x.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cbmem.h>
#include "chip.h"
#include "nehalem.h"
#include <cpu/intel/smm/gen1/smi.h>

static int bridge_revision_id = -1;

int bridge_silicon_revision(void)
{
	if (bridge_revision_id < 0) {
		uint8_t stepping = cpuid_eax(1) & 0xf;
		uint8_t bridge_id =
		    pci_read_config16(dev_find_slot(0, PCI_DEVFN(0, 0)),
				      PCI_DEVICE_ID) & 0xf0;
		bridge_revision_id = bridge_id | stepping;
	}
	return bridge_revision_id;
}

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

	/* 0xe0000000-0xf0000000 PCIe config.
	   0xfed10000-0xfed14000 MCH
	   0xfed17000-0xfed18000 HECI
	   0xfed18000-0xfed19000 DMI
	   0xfed19000-0xfed1a000 EPBAR
	   0xfed1c000-0xfed20000 RCBA
	   0xfed90000-0xfed94000 IOMMU
	   0xff800000-0xffffffff ROM. */

	resource = new_resource(dev, index++);
	resource->base = (resource_t) 0xfed00000;
	resource->size = (resource_t) 0x00100000;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
	  IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	mmio_resource(dev, index++, legacy_hole_base_k,
		      (0xc0000 >> 10) - legacy_hole_base_k);
	reserved_ram_resource(dev, index++, 0xc0000 >> 10,
			      (0x100000 - 0xc0000) >> 10);

#if IS_ENABLED(CONFIG_CHROMEOS_RAMOOPS)
	reserved_ram_resource(dev, index++,
			      CONFIG_CHROMEOS_RAMOOPS_RAM_START >> 10,
			      CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE >> 10);
#endif
}

static void pci_domain_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
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
#endif

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.enable_resources = NULL,
	.init = NULL,
	.scan_bus = pci_domain_scan_bus,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name = northbridge_acpi_name,
#endif
};

static void mc_read_resources(struct device *dev)
{
	uint32_t tseg_base;
	uint64_t TOUUD;
	uint16_t reg16;

	pci_dev_read_resources(dev);

	mmconf_resource(dev, 0x50);

	tseg_base = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0, 0)), TSEG);
	TOUUD = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0, 0)),
				  D0F0_TOUUD);

	printk(BIOS_DEBUG, "ram_before_4g_top: 0x%x\n", tseg_base);
	printk(BIOS_DEBUG, "TOUUD: 0x%x\n", (unsigned)TOUUD);

	/* Report the memory regions */
	ram_resource(dev, 3, 0, 640);
	ram_resource(dev, 4, 768, ((tseg_base >> 10) - 768));

	mmio_resource(dev, 5, tseg_base >> 10, CONFIG_SMM_TSEG_SIZE >> 10);

	reg16 = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0, 0)), D0F0_GGC);
	const int uma_sizes_gtt[16] =
	    { 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 3, 4, 42, 42, 42, 42 };
	/* Igd memory */
	const int uma_sizes_igd[16] = {
		0, 0, 0, 0, 0, 32, 48, 64, 128, 256, 96, 160, 224, 352, 256, 512
	};
	u32 igd_base, gtt_base;
	int uma_size_igd, uma_size_gtt;

	uma_size_igd = uma_sizes_igd[(reg16 >> 4) & 0xF];
	uma_size_gtt = uma_sizes_gtt[(reg16 >> 8) & 0xF];

	igd_base =
	    pci_read_config32(dev_find_slot(0, PCI_DEVFN(0, 0)), D0F0_IGD_BASE);
	gtt_base =
	    pci_read_config32(dev_find_slot(0, PCI_DEVFN(0, 0)), D0F0_GTT_BASE);
	mmio_resource(dev, 6, gtt_base >> 10, uma_size_gtt << 10);
	mmio_resource(dev, 7, igd_base >> 10, uma_size_igd << 10);

	if (TOUUD > 4096)
		ram_resource(dev, 8, (4096 << 10), ((TOUUD - 4096) << 10));

	/* This memory is not DMA-capable. */
	if (TOUUD >= 8192 - 64)
	    bad_ram_resource(dev, 9, 0x1fc000000ULL >> 10, 0x004000000 >> 10);

	add_fixed_resources(dev, 10);
}

u32 northbridge_get_tseg_base(void)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0, 0));

	return pci_read_config32(dev, TSEG) & ~1;
}

u32 northbridge_get_tseg_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

static void mc_set_resources(struct device *dev)
{
	/* And call the normal set_resources */
	pci_dev_set_resources(dev);
}

static void intel_set_subsystem(struct device *dev, unsigned int vendor,
				unsigned int device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   ((device & 0xffff) << 16) | (vendor &
								0xffff));
	}
}

static void northbridge_dmi_init(struct device *dev)
{
	u32 reg32;

	/* Clear error status bits */
	DMIBAR32(0x1c4) = 0xffffffff;
	DMIBAR32(0x1d0) = 0xffffffff;

	/* Steps prior to DMI ASPM */
	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		reg32 = DMIBAR32(0x250);
		reg32 &= ~((1 << 22) | (1 << 20));
		reg32 |= (1 << 21);
		DMIBAR32(0x250) = reg32;
	}

	reg32 = DMIBAR32(0x238);
	reg32 |= (1 << 29);
	DMIBAR32(0x238) = reg32;

	if (bridge_silicon_revision() >= SNB_STEP_D0) {
		reg32 = DMIBAR32(0x1f8);
		reg32 |= (1 << 16);
		DMIBAR32(0x1f8) = reg32;
	} else if (bridge_silicon_revision() >= SNB_STEP_D1) {
		reg32 = DMIBAR32(0x1f8);
		reg32 &= ~(1 << 26);
		reg32 |= (1 << 16);
		DMIBAR32(0x1f8) = reg32;

		reg32 = DMIBAR32(0x1fc);
		reg32 |= (1 << 12) | (1 << 23);
		DMIBAR32(0x1fc) = reg32;
	}

	/* Enable ASPM on SNB link, should happen before PCH link */
	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		reg32 = DMIBAR32(0xd04);
		reg32 |= (1 << 4);
		DMIBAR32(0xd04) = reg32;
	}

	reg32 = DMIBAR32(0x88);
	reg32 |= (1 << 1) | (1 << 0);
	DMIBAR32(0x88) = reg32;
}

static void northbridge_init(struct device *dev)
{
	u8 bios_reset_cpl;
	u32 bridge_type;

	northbridge_dmi_init(dev);

	bridge_type = MCHBAR32(0x5f10);
	bridge_type &= ~0xff;

	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_IVB) {
		/* Enable Power Aware Interrupt Routing */
		u8 pair = MCHBAR8(0x5418);
		pair &= ~0xf;	/* Clear 3:0 */
		pair |= 0x4;	/* Fixed Priority */
		MCHBAR8(0x5418) = pair;

		/* 30h for IvyBridge */
		bridge_type |= 0x30;
	} else {
		/* 20h for Sandybridge */
		bridge_type |= 0x20;
	}
	MCHBAR32(0x5f10) = bridge_type;

	/*
	 * Set bit 0 of BIOS_RESET_CPL to indicate to the CPU
	 * that BIOS has initialized memory and power management
	 */
	bios_reset_cpl = MCHBAR8(BIOS_RESET_CPL);
	bios_reset_cpl |= 1;
	MCHBAR8(BIOS_RESET_CPL) = bios_reset_cpl;
	printk(BIOS_DEBUG, "Set BIOS_RESET_CPL\n");

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
#ifdef DISABLED
	set_power_limits(28);

	/*
	 * CPUs with configurable TDP also need power limits set
	 * in MCHBAR.  Use same values from MSR_PKG_POWER_LIMIT.
	 */
	if (cpu_config_tdp_levels()) {
		msr_t msr = rdmsr(MSR_PKG_POWER_LIMIT);
		MCHBAR32(0x59A0) = msr.lo;
		MCHBAR32(0x59A4) = msr.hi;
	}
#endif
	/* Set here before graphics PM init */
	MCHBAR32(0x5500) = 0x00100001;
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem = intel_set_subsystem,
};

static struct device_operations mc_ops = {
	.read_resources = mc_read_resources,
	.set_resources = mc_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = northbridge_init,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
	.scan_bus = 0,
	.ops_pci = &intel_pci_ops,
};

static const struct pci_driver mc_driver_44 __pci_driver = {
	.ops = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0044,	/* Nehalem */
};

static void cpu_bus_init(struct device *dev)
{
	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = cpu_bus_init,
	.scan_bus = 0,
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

struct chip_operations northbridge_intel_nehalem_ops = {
	CHIP_NAME("Intel i7 (Nehalem) integrated Northbridge")
	    .enable_dev = enable_dev,
};
