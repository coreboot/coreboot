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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <stdint.h>
#include <delay.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cbmem.h>
#include "chip.h"
#include "sandybridge.h"

static int bridge_revision_id = -1;

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

unsigned long get_top_of_ram(void)
{
	/* Base of TSEG is top of usable DRAM */
	u32 tom = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0,0)), TSEG);
	return (unsigned long) tom;
}

/* Reserve everything between A segment and 1MB:
 *
 * 0xa0000 - 0xbffff: legacy VGA
 * 0xc0000 - 0xcffff: VGA OPROM (needed by kernel)
 * 0xe0000 - 0xfffff: SeaBIOS, if used, otherwise DMI
 */
static const int legacy_hole_base_k = 0xa0000 / 1024;
static const int legacy_hole_size_k = 384;

static int get_pcie_bar(u32 *base, u32 *len)
{
	device_t dev;
	u32 pciexbar_reg;

	*base = 0;
	*len = 0;

	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!dev)
		return 0;

	pciexbar_reg = pci_read_config32(dev, PCIEXBAR);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28));
		*len = 256 * 1024 * 1024;
		return 1;
	case 1: // 128M
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27));
		*len = 128 * 1024 * 1024;
		return 1;
	case 2: // 64M
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27)|(1 << 26));
		*len = 64 * 1024 * 1024;
		return 1;
	}

	return 0;
}

static void add_fixed_resources(struct device *dev, int index)
{
	struct resource *resource;
	u32 pcie_config_base, pcie_config_size;

	/* Using uma_resource() here would fail as base & size cannot
	 * be used as-is for a single MTRR. This would cause excessive
	 * use of MTRRs.
	 *
	 * Use of mmio_resource() instead does not create UC holes by using
	 * MTRRs, but making these regions uncacheable is taken care of by
	 * making sure they do not overlap with any ram_resource().
	 *
	 * The resources can be changed to use separate mmio_resource()
	 * calls after MTRR code is able to merge them wisely.
	 */
	mmio_resource(dev, index++, uma_memory_base >> 10, uma_memory_size >> 10);

	if (get_pcie_bar(&pcie_config_base, &pcie_config_size)) {
		printk(BIOS_DEBUG, "Adding PCIe config bar base=0x%08x "
		       "size=0x%x\n", pcie_config_base, pcie_config_size);
		resource = new_resource(dev, index++);
		resource->base = (resource_t) pcie_config_base;
		resource->size = (resource_t) pcie_config_size;
		resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		    IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	}

	mmio_resource(dev, index++, legacy_hole_base_k,
			(0xc0000 >> 10) - legacy_hole_base_k);
	reserved_ram_resource(dev, index++, 0xc0000 >> 10,
			(0x100000 - 0xc0000) >> 10);

#if CONFIG_CHROMEOS_RAMOOPS
	reserved_ram_resource(dev, index++,
			CONFIG_CHROMEOS_RAMOOPS_RAM_START >> 10,
			CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE >> 10);
#endif

	/* Required for SandyBridge sighting 3715511 */
	bad_ram_resource(dev, index++, 0x20000000 >> 10, 0x00200000 >> 10);
	bad_ram_resource(dev, index++, 0x40000000 >> 10, 0x00200000 >> 10);
}

static void pci_domain_set_resources(device_t dev)
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

	set_top_of_ram(tomk * 1024);
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
};

static void mc_read_resources(device_t dev)
{
	struct resource *resource;

	pci_dev_read_resources(dev);

	/* So, this is one of the big mysteries in the coreboot resource
	 * allocator. This resource should make sure that the address space
	 * of the PCIe memory mapped config space bar. But it does not.
	 */

	/* We use 0xcf as an unused index for our PCIe bar so that we find it again */
	resource = new_resource(dev, 0xcf);
	resource->base = DEFAULT_PCIEXBAR;
	resource->size = 64 * 1024 * 1024;	/* 64MB hard coded PCIe config space */
	resource->flags =
	    IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
	    IORESOURCE_ASSIGNED;
	printk(BIOS_DEBUG, "Adding PCIe enhanced config space BAR 0x%08lx-0x%08lx.\n",
		     (unsigned long)(resource->base), (unsigned long)(resource->base + resource->size));
}

static void mc_set_resources(device_t dev)
{
	struct resource *resource;

	/* Report the PCIe BAR */
	resource = find_resource(dev, 0xcf);
	if (resource) {
		report_resource_stored(dev, resource, "<mmconfig>");
	}

	/* And call the normal set_resources */
	pci_dev_set_resources(dev);
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

static void northbridge_enable(device_t dev)
{
#if CONFIG_HAVE_ACPI_RESUME
	switch (pci_read_config32(dev, SKPAD)) {
	case 0xcafebabe:
		printk(BIOS_DEBUG, "Normal boot.\n");
		acpi_slp_type=0;
		break;
	case 0xcafed00d:
		printk(BIOS_DEBUG, "S3 Resume.\n");
		acpi_slp_type=3;
		break;
	default:
		printk(BIOS_DEBUG, "Unknown boot method, assuming normal.\n");
		acpi_slp_type=0;
		break;
	}
#endif
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem    = intel_set_subsystem,
};

static struct device_operations mc_ops = {
	.read_resources   = mc_read_resources,
	.set_resources    = mc_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init,
	.enable           = northbridge_enable,
	.scan_bus         = 0,
	.ops_pci          = &intel_pci_ops,
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

static const struct pci_driver mc_driver_1 __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0154, /* Ivy bridge */
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
	/* Enable ROM caching if option was selected. */
	x86_mtrr_enable_rom_caching();
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources   = cpu_bus_noop,
	.set_resources    = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
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

struct chip_operations northbridge_intel_sandybridge_ops = {
	CHIP_NAME("Intel i7 (SandyBridge/IvyBridge) integrated Northbridge")
	.enable_dev = enable_dev,
};
