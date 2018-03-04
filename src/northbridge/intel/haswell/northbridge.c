/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#include <cpu/intel/haswell/haswell.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/smm.h>
#include <boot/tables.h>
#include <cbmem.h>
#include "chip.h"
#include "haswell.h"

static int get_pcie_bar(device_t dev, unsigned int index, u32 *base, u32 *len)
{
	u32 pciexbar_reg;
	u32 mask;

	*base = 0;
	*len = 0;

	pciexbar_reg = pci_read_config32(dev, index);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		mask = (1UL << 31) | (1 << 30) | (1 << 29) | (1 << 28);
		*base = pciexbar_reg & mask;
		*len = 256 * 1024 * 1024;
		return 1;
	case 1: // 128M
		mask = (1UL << 31) | (1 << 30) | (1 << 29) | (1 << 28);
		mask |= (1 << 27);
		*base = pciexbar_reg & mask;
		*len = 128 * 1024 * 1024;
		return 1;
	case 2: // 64M
		mask = (1UL << 31) | (1 << 30) | (1 << 29) | (1 << 28);
		mask |= (1 << 27) | (1 << 26);
		*base = pciexbar_reg & mask;
		*len = 64 * 1024 * 1024;
		return 1;
	}

	return 0;
}

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

	/* TODO We could determine how many PCIe busses we need in
	 * the bar. For now that number is hardcoded to a max of 64.
	 * See e7525/northbridge.c for an example.
	 */
static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci_bus	  = pci_bus_default_ops,
	.write_acpi_tables = northbridge_write_acpi_tables,
};

static int get_bar(device_t dev, unsigned int index, u32 *base, u32 *len)
{
	u32 bar;

	bar = pci_read_config32(dev, index);

	/* If not enabled don't report it. */
	if (!(bar & 0x1))
		return 0;

	/* Knock down the enable bit. */
	*base = bar & ~1;

	return 1;
}

/* There are special BARs that actually are programmed in the MCHBAR. These
 * Intel special features, but they do consume resources that need to be
 * accounted for. */
static int get_bar_in_mchbar(device_t dev, unsigned int index, u32 *base, u32 *len)
{
	u32 bar;

	bar = MCHBAR32(index);

	/* If not enabled don't report it. */
	if (!(bar & 0x1))
		return 0;

	/* Knock down the enable bit. */
	*base = bar & ~1;

	return 1;
}

struct fixed_mmio_descriptor {
	unsigned int index;
	u32 size;
	int (*get_resource)(device_t dev, unsigned int index,
			    u32 *base, u32 *size);
	const char *description;
};

#define SIZE_KB(x) ((x)*1024)
struct fixed_mmio_descriptor mc_fixed_resources[] = {
	{ PCIEXBAR, SIZE_KB(0),  get_pcie_bar,      "PCIEXBAR" },
	{ MCHBAR,   SIZE_KB(32), get_bar,           "MCHBAR"   },
	{ DMIBAR,   SIZE_KB(4),  get_bar,           "DMIBAR"   },
	{ EPBAR,    SIZE_KB(4),  get_bar,           "EPBAR"    },
	{ 0x5420,   SIZE_KB(4),  get_bar_in_mchbar, "GDXCBAR"  },
	{ 0x5408,   SIZE_KB(16), get_bar_in_mchbar, "EDRAMBAR" },
};
#undef SIZE_KB

/*
 * Add all known fixed MMIO ranges that hang off the host bridge/memory
 * controller device.
 */
static void mc_add_fixed_mmio_resources(device_t dev)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mc_fixed_resources); i++) {
		u32 base;
		u32 size;
		struct resource *resource;
		unsigned int index;

		size = mc_fixed_resources[i].size;
		index = mc_fixed_resources[i].index;
		if (!mc_fixed_resources[i].get_resource(dev, index,
							&base, &size))
			continue;

		resource = new_resource(dev, mc_fixed_resources[i].index);
		resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
				  IORESOURCE_STORED | IORESOURCE_RESERVE |
				  IORESOURCE_ASSIGNED;
		resource->base = base;
		resource->size = size;
		printk(BIOS_DEBUG, "%s: Adding %s @ %x 0x%08lx-0x%08lx.\n",
		       __func__, mc_fixed_resources[i].description, index,
		       (unsigned long)base, (unsigned long)(base + size - 1));
	}
}

/* Host Memory Map:
 *
 * +--------------------------+ TOUUD
 * |                          |
 * +--------------------------+ 4GiB
 * |     PCI Address Space    |
 * +--------------------------+ TOLUD (also maps into MC address space)
 * |     iGD                  |
 * +--------------------------+ BDSM
 * |     GTT                  |
 * +--------------------------+ BGSM
 * |     TSEG                 |
 * +--------------------------+ TSEGMB
 * |     Usage DRAM           |
 * +--------------------------+ 0
 *
 * Some of the base registers above can be equal making the size of those
 * regions 0. The reason is because the memory controller internally subtracts
 * the base registers from each other to determine sizes of the regions. In
 * other words, the memory map is in a fixed order no matter what.
 */

struct map_entry {
	int reg;
	int is_64_bit;
	int is_limit;
	const char *description;
};

static void read_map_entry(device_t dev, struct map_entry *entry, uint64_t *result)
{
	uint64_t value;
	uint64_t mask;

	/* All registers are on a 1MiB granularity. */
	mask = ((1ULL<<20)-1);
	mask = ~mask;

	value = 0;

	if (entry->is_64_bit) {
		value = pci_read_config32(dev, entry->reg + 4);
		value <<= 32;
	}

	value |= pci_read_config32(dev, entry->reg);
	value &= mask;

	if (entry->is_limit)
		value |= ~mask;

	*result = value;
}

#define MAP_ENTRY(reg_, is_64_, is_limit_, desc_) \
	{ \
		.reg = reg_,           \
		.is_64_bit = is_64_,   \
		.is_limit = is_limit_, \
		.description = desc_,  \
	}

#define MAP_ENTRY_BASE_64(reg_, desc_) \
	MAP_ENTRY(reg_, 1, 0, desc_)
#define MAP_ENTRY_LIMIT_64(reg_, desc_) \
	MAP_ENTRY(reg_, 1, 1, desc_)
#define MAP_ENTRY_BASE_32(reg_, desc_) \
	MAP_ENTRY(reg_, 0, 0, desc_)

enum {
	TOM_REG,
	TOUUD_REG,
	MESEG_BASE_REG,
	MESEG_LIMIT_REG,
	REMAP_BASE_REG,
	REMAP_LIMIT_REG,
	TOLUD_REG,
	BGSM_REG,
	BDSM_REG,
	TSEG_REG,
	// Must be last.
	NUM_MAP_ENTRIES
};

static struct map_entry memory_map[NUM_MAP_ENTRIES] = {
	[TOM_REG] = MAP_ENTRY_BASE_64(TOM, "TOM"),
	[TOUUD_REG] = MAP_ENTRY_BASE_64(TOUUD, "TOUUD"),
	[MESEG_BASE_REG] = MAP_ENTRY_BASE_64(MESEG_BASE, "MESEG_BASE"),
	[MESEG_LIMIT_REG] = MAP_ENTRY_LIMIT_64(MESEG_LIMIT, "MESEG_LIMIT"),
	[REMAP_BASE_REG] = MAP_ENTRY_BASE_64(REMAPBASE, "REMAP_BASE"),
	[REMAP_LIMIT_REG] = MAP_ENTRY_LIMIT_64(REMAPLIMIT, "REMAP_LIMIT"),
	[TOLUD_REG] = MAP_ENTRY_BASE_32(TOLUD, "TOLUD"),
	[BDSM_REG] = MAP_ENTRY_BASE_32(BDSM, "BDSM"),
	[BGSM_REG] = MAP_ENTRY_BASE_32(BGSM, "BGSM"),
	[TSEG_REG] = MAP_ENTRY_BASE_32(TSEG, "TESGMB"),
};

static void mc_read_map_entries(device_t dev, uint64_t *values)
{
	int i;
	for (i = 0; i < NUM_MAP_ENTRIES; i++) {
		read_map_entry(dev, &memory_map[i], &values[i]);
	}
}

static void mc_report_map_entries(device_t dev, uint64_t *values)
{
	int i;
	for (i = 0; i < NUM_MAP_ENTRIES; i++) {
		printk(BIOS_DEBUG, "MC MAP: %s: 0x%llx\n",
		       memory_map[i].description, values[i]);
	}
	/* One can validate the BDSM and BGSM against the GGC. */
	printk(BIOS_DEBUG, "MC MAP: GGC: 0x%x\n", pci_read_config16(dev, GGC));
}

static void mc_add_dram_resources(device_t dev, int *resource_cnt)
{
	unsigned long base_k, size_k;
	unsigned long touud_k;
	unsigned long index;
	struct resource *resource;
	uint64_t mc_values[NUM_MAP_ENTRIES];

	/* Read in the MAP registers and report their values. */
	mc_read_map_entries(dev, &mc_values[0]);
	mc_report_map_entries(dev, &mc_values[0]);

	/*
	 * These are the host memory ranges that should be added:
	 * - 0 -> 0xa0000: cacheable
	 * - 0xc0000 -> TSEG : cacheable
	 * - TESG -> BGSM: cacheable with standard MTRRs and reserved
	 * - BGSM -> TOLUD: not cacheable with standard MTRRs and reserved
	 * - 4GiB -> TOUUD: cacheable
	 *
	 * The default SMRAM space is reserved so that the range doesn't
	 * have to be saved during S3 Resume. Once marked reserved the OS
	 * cannot use the memory. This is a bit of an odd place to reserve
	 * the region, but the CPU devices don't have dev_ops->read_resources()
	 * called on them.
	 *
	 * The range 0xa0000 -> 0xc0000 does not have any resources
	 * associated with it to handle legacy VGA memory. If this range
	 * is not omitted the mtrr code will setup the area as cacheable
	 * causing VGA access to not work.
	 *
	 * The TSEG region is mapped as cacheable so that one can perform
	 * SMRAM relocation faster. Once the SMRR is enabled the SMRR takes
	 * precedence over the existing MTRRs covering this region.
	 *
	 * It should be noted that cacheable entry types need to be added in
	 * order. The reason is that the current MTRR code assumes this and
	 * falls over itself if it isn't.
	 *
	 * The resource index starts low and should not meet or exceed
	 * PCI_BASE_ADDRESS_0.
	 */
	index = *resource_cnt;

	/* 0 - > 0xa0000 */
	base_k = 0;
	size_k = (0xa0000 >> 10) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* 0xc0000 -> TSEG */
	base_k = 0xc0000 >> 10;
	size_k = (unsigned long)(mc_values[TSEG_REG] >> 10) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* TSEG -> BGSM */
	resource = new_resource(dev, index++);
	resource->base = mc_values[TSEG_REG];
	resource->size = mc_values[BGSM_REG] - resource->base;
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
			  IORESOURCE_STORED | IORESOURCE_RESERVE |
			  IORESOURCE_ASSIGNED | IORESOURCE_CACHEABLE;

	/* BGSM -> TOLUD */
	resource = new_resource(dev, index++);
	resource->base = mc_values[BGSM_REG];
	resource->size = mc_values[TOLUD_REG] - resource->base;
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
			  IORESOURCE_STORED | IORESOURCE_RESERVE |
			  IORESOURCE_ASSIGNED;

	/* 4GiB -> TOUUD */
	base_k = 4096 * 1024; /* 4GiB */
	touud_k = mc_values[TOUUD_REG] >> 10;
	size_k = touud_k - base_k;
	if (touud_k > base_k)
		ram_resource(dev, index++, base_k, size_k);

	/* Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource(dev, index++, (0xa0000 >> 10), (0xc0000 - 0xa0000) >> 10);
	reserved_ram_resource(dev, index++, (0xc0000 >> 10),
			      (0x100000 - 0xc0000) >> 10);
#if IS_ENABLED(CONFIG_CHROMEOS_RAMOOPS)
	reserved_ram_resource(dev, index++,
			CONFIG_CHROMEOS_RAMOOPS_RAM_START >> 10,
			CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE >> 10);
#endif
	*resource_cnt = index;
}

static void mc_read_resources(device_t dev)
{
	int index = 0;
	const bool vtd_capable =
		!(pci_read_config32(dev, CAPID0_A) & VTD_DISABLE);

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* Add all fixed MMIO resources. */
	mc_add_fixed_mmio_resources(dev);

	/* Add VT-d MMIO resources if capable */
	if (vtd_capable) {
		mmio_resource(dev, index++, GFXVT_BASE_ADDRESS / KiB,
			GFXVT_BASE_SIZE / KiB);
		mmio_resource(dev, index++, VTVC0_BASE_ADDRESS / KiB,
			VTVC0_BASE_SIZE / KiB);
	}

	/* Calculate and add DRAM resources. */
	mc_add_dram_resources(dev, &index);
}

static void intel_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static void northbridge_init(struct device *dev)
{
	u8 bios_reset_cpl, pair;

	/* Enable Power Aware Interrupt Routing */
	pair = MCHBAR8(0x5418);
	pair &= ~0x7;	/* Clear 2:0 */
	pair |= 0x4;	/* Fixed Priority */
	MCHBAR8(0x5418) = pair;

	/*
	 * Set bits 0+1 of BIOS_RESET_CPL to indicate to the CPU
	 * that BIOS has initialized memory and power management
	 */
	bios_reset_cpl = MCHBAR8(BIOS_RESET_CPL);
	bios_reset_cpl |= 3;
	MCHBAR8(BIOS_RESET_CPL) = bios_reset_cpl;
	printk(BIOS_DEBUG, "Set BIOS_RESET_CPL\n");

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
	set_power_limits(28);

	/* Set here before graphics PM init */
	MCHBAR32(0x5500) = 0x00100001;
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem    = intel_set_subsystem,
};

static struct device_operations mc_ops = {
	.read_resources   = mc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
	.scan_bus         = 0,
	.ops_pci          = &intel_pci_ops,
};

static const struct pci_driver mc_driver_hsw_mobile __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_HSW_MOBILE,
};

static const struct pci_driver mc_driver_hsw_ult __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_HSW_ULT,
};

static void cpu_bus_init(device_t dev)
{
	bsp_init_and_start_aps(dev->link_list);
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
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_intel_haswell_ops = {
	CHIP_NAME("Intel i7 (Haswell) integrated Northbridge")
	.enable_dev = enable_dev,
};
