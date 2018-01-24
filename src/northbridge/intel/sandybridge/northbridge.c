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
#include <cpu/intel/model_206ax/model_206ax.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cbmem.h>
#include "chip.h"
#include "sandybridge.h"
#include <cpu/intel/smm/gen1/smi.h>

static int bridge_revision_id = -1;

/* IGD UMA memory */
static uint64_t uma_memory_base = 0;
static uint64_t uma_memory_size = 0;

int bridge_silicon_revision(void)
{
	if (bridge_revision_id < 0) {
		uint8_t stepping = cpuid_eax(1) & 0xf;
		uint8_t bridge_id = pci_read_config16(
			dev_find_slot(0, PCI_DEVFN(0, 0)),
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
static const int legacy_hole_size_k = 384;

static int get_pcie_bar(u32 *base)
{
	struct device *dev;
	u32 pciexbar_reg;

	*base = 0;

	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!dev)
		return 0;

	pciexbar_reg = pci_read_config32(dev, PCIEXBAR);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28));
		return 256;
	case 1: // 128M
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27));
		return 128;
	case 2: // 64M
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27)|(1 << 26));
		return 64;
	}

	return 0;
}

static void add_fixed_resources(struct device *dev, int index)
{
	mmio_resource(dev, index++, uma_memory_base >> 10, uma_memory_size >> 10);

	mmio_resource(dev, index++, legacy_hole_base_k,
			(0xc0000 >> 10) - legacy_hole_base_k);
	reserved_ram_resource(dev, index++, 0xc0000 >> 10,
			(0x100000 - 0xc0000) >> 10);

#if IS_ENABLED(CONFIG_CHROMEOS_RAMOOPS)
	reserved_ram_resource(dev, index++,
			CONFIG_CHROMEOS_RAMOOPS_RAM_START >> 10,
			CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE >> 10);
#endif

	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		/* Required for SandyBridge sighting 3715511 */
		bad_ram_resource(dev, index++, 0x20000000 >> 10, 0x00200000 >> 10);
		bad_ram_resource(dev, index++, 0x40000000 >> 10, 0x00200000 >> 10);
	}

	/* Reserve IOMMU BARs */
	const u32 capid0_a = pci_read_config32(dev, 0xe4);
	if (!(capid0_a & (1 << 23))) {
		mmio_resource(dev, index++, IOMMU_BASE1 >> 10, 4);
		mmio_resource(dev, index++, IOMMU_BASE2 >> 10, 4);
	}
}

static void pci_domain_set_resources(struct device *dev)
{
	uint64_t tom, me_base, touud;
	uint32_t tseg_base, uma_size, tolud;
	uint16_t ggc;
	unsigned long long tomk;

	/* Total Memory 2GB example:
	 *
	 *  00000000  0000MB-1992MB  1992MB  RAM     (writeback)
	 *  7c800000  1992MB-2000MB     8MB  TSEG    (SMRR)
	 *  7d000000  2000MB-2002MB     2MB  GFX GTT (uncached)
	 *  7d200000  2002MB-2034MB    32MB  GFX UMA (uncached)
	 *  7f200000   2034MB TOLUD
	 *  7f800000   2040MB MEBASE
	 *  7f800000  2040MB-2048MB     8MB  ME UMA  (uncached)
	 *  80000000   2048MB TOM
	 * 100000000  4096MB-4102MB     6MB  RAM     (writeback)
	 *
	 * Total Memory 4GB example:
	 *
	 *  00000000  0000MB-2768MB  2768MB  RAM     (writeback)
	 *  ad000000  2768MB-2776MB     8MB  TSEG    (SMRR)
	 *  ad800000  2776MB-2778MB     2MB  GFX GTT (uncached)
	 *  ada00000  2778MB-2810MB    32MB  GFX UMA (uncached)
	 *  afa00000   2810MB TOLUD
	 *  ff800000   4088MB MEBASE
	 *  ff800000  4088MB-4096MB     8MB  ME UMA  (uncached)
	 * 100000000   4096MB TOM
	 * 100000000  4096MB-5374MB  1278MB  RAM     (writeback)
	 * 14fe00000   5368MB TOUUD
	 */

	/* Top of Upper Usable DRAM, including remap */
	touud = pci_read_config32(dev, TOUUD+4);
	touud <<= 32;
	touud |= pci_read_config32(dev, TOUUD);

	/* Top of Lower Usable DRAM */
	tolud = pci_read_config32(dev, TOLUD);

	/* Top of Memory - does not account for any UMA */
	tom = pci_read_config32(dev, 0xa4);
	tom <<= 32;
	tom |= pci_read_config32(dev, 0xa0);

	printk(BIOS_DEBUG, "TOUUD 0x%llx TOLUD 0x%08x TOM 0x%llx\n",
	       touud, tolud, tom);

	/* ME UMA needs excluding if total memory <4GB */
	me_base = pci_read_config32(dev, 0x74);
	me_base <<= 32;
	me_base |= pci_read_config32(dev, 0x70);

	printk(BIOS_DEBUG, "MEBASE 0x%llx\n", me_base);

	uma_memory_base = tolud;
	tomk = tolud >> 10;
	if (me_base == tolud) {
		/* ME is from MEBASE-TOM */
		uma_size = (tom - me_base) >> 10;
		/* Increment TOLUD to account for ME as RAM */
		tolud += uma_size << 10;
		/* UMA starts at old TOLUD */
		uma_memory_base = tomk * 1024ULL;
		uma_memory_size = uma_size * 1024ULL;
		printk(BIOS_DEBUG, "ME UMA base 0x%llx size %uM\n",
		       me_base, uma_size >> 10);
	}

	/* Graphics memory comes next */
	ggc = pci_read_config16(dev, GGC);
	if (!(ggc & 2)) {
		printk(BIOS_DEBUG, "IGD decoded, subtracting ");

		/* Graphics memory */
		uma_size = ((ggc >> 3) & 0x1f) * 32 * 1024ULL;
		printk(BIOS_DEBUG, "%uM UMA", uma_size >> 10);
		tomk -= uma_size;
		uma_memory_base = tomk * 1024ULL;
		uma_memory_size += uma_size * 1024ULL;

		/* GTT Graphics Stolen Memory Size (GGMS) */
		uma_size = ((ggc >> 8) & 0x3) * 1024ULL;
		tomk -= uma_size;
		uma_memory_base = tomk * 1024ULL;
		uma_memory_size += uma_size * 1024ULL;
		printk(BIOS_DEBUG, " and %uM GTT\n", uma_size >> 10);
	}

	/* Calculate TSEG size from its base which must be below GTT */
	tseg_base = pci_read_config32(dev, 0xb8);
	uma_size = (uma_memory_base - tseg_base) >> 10;
	tomk -= uma_size;
	uma_memory_base = tomk * 1024ULL;
	uma_memory_size += uma_size * 1024ULL;
	printk(BIOS_DEBUG, "TSEG base 0x%08x size %uM\n",
	       tseg_base, uma_size >> 10);

	printk(BIOS_INFO, "Available memory below 4GB: %lluM\n", tomk >> 10);

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

	add_fixed_resources(dev, 6);

	assign_resources(dev->link_list);
}

static const char *northbridge_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0, 0):
		return "MCHC";
	}

	return NULL;
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
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_name        = northbridge_acpi_name,
};

static void mc_read_resources(struct device *dev)
{
	u32 pcie_config_base;
	int buses;

	pci_dev_read_resources(dev);

	buses = get_pcie_bar(&pcie_config_base);
	if (buses) {
		struct resource *resource = new_resource(dev, PCIEXBAR);
		mmconf_resource_init(resource, pcie_config_base, buses);
	}
}

static void intel_set_subsystem(struct device *dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
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
		reg32 &= ~((1 << 22)|(1 << 20));
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

/* Disable unused PEG devices based on devicetree */
static void disable_peg(void)
{
	struct device *dev;
	u32 reg;

	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	reg = pci_read_config32(dev, DEVEN);

	dev = dev_find_slot(0, PCI_DEVFN(1, 2));
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG12.\n");
		reg &= ~DEVEN_PEG12;
	}
	dev = dev_find_slot(0, PCI_DEVFN(1, 1));
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG11.\n");
		reg &= ~DEVEN_PEG11;
	}
	dev = dev_find_slot(0, PCI_DEVFN(1, 0));
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG10.\n");
		reg &= ~DEVEN_PEG10;
	}
	dev = dev_find_slot(0, PCI_DEVFN(2, 0));
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling IGD.\n");
		reg &= ~DEVEN_IGD;
	}
	dev = dev_find_slot(0, PCI_DEVFN(4, 0));
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling Device 4.\n");
		reg &= ~DEVEN_D4EN;
	}
	dev = dev_find_slot(0, PCI_DEVFN(6, 0));
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG60.\n");
		reg &= ~DEVEN_PEG60;
	}
	dev = dev_find_slot(0, PCI_DEVFN(7, 0));
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling Device 7.\n");
		reg &= ~DEVEN_D7EN;
	}

	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	pci_write_config32(dev, DEVEN, reg);
	if (!(reg & (DEVEN_PEG60 | DEVEN_PEG10 | DEVEN_PEG11 | DEVEN_PEG12))) {
		/* Set the PEG clock gating bit.
		 * Disables the IO clock on all PEG devices. */
		MCHBAR32(0x7010) = MCHBAR32(0x7010) | 0x01;
		printk(BIOS_DEBUG, "Disabling PEG IO clock.\n");
	}
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

	/* Turn off unused devices. Has to be done before
	 * setting BIOS_RESET_CPL.
	 */
	disable_peg();

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

	/* Set here before graphics PM init */
	MCHBAR32(0x5500) = 0x00100001;
}

static u32 northbridge_get_base_reg(struct device *dev, int reg)
{
	u32 value;

	value = pci_read_config32(dev, reg);
	/* Base registers are at 1MiB granularity. */
	value &= ~((1 << 20) - 1);
	return value;
}

u32 northbridge_get_tseg_base(void)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0, 0));

	return northbridge_get_base_reg(dev, TSEG);
}

u32 northbridge_get_tseg_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

void northbridge_write_smram(u8 smram)
{
	pci_write_config8(dev_find_slot(0, PCI_DEVFN(0, 0)), SMRAM, smram);
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem    = intel_set_subsystem,
};

static struct device_operations mc_ops = {
	.read_resources   = mc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init,
	.scan_bus         = 0,
	.ops_pci          = &intel_pci_ops,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
};

static const struct pci_driver mc_driver_0100 __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0100,
};

static const struct pci_driver mc_driver __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0104, /* Sandy bridge */
};

static const struct pci_driver mc_driver_150 __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0150, /* Ivy bridge */
};

static const struct pci_driver mc_driver_1 __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0154, /* Ivy bridge */
};

static const struct pci_driver mc_driver_158 __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0158, /* Ivy bridge */
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

struct chip_operations northbridge_intel_sandybridge_ops = {
	CHIP_NAME("Intel SandyBridge/IvyBridge integrated Northbridge")
	.enable_dev = enable_dev,
};
