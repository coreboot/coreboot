/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#include <arch/acpi.h>
#include <arch/io.h>
#include <stdint.h>
#include <delay.h>
#include <cpu/intel/fsp_model_406dx/model_406dx.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cbmem.h>
#include "chip.h"
#include "northbridge.h"
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <cpu/x86/lapic.h>

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

	pciexbar_reg = sideband_read(B_UNIT, BECREG);

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
	u32 tomlow, bmbound, bsmmrrl, bsmmrrh;
	u64 bmbound_hi;
	int index = 0;

	/*
	 *  These are the host memory ranges :
	 * - 0 -> SMM (SMMRRL) : cacheable
	 * - SMM -> LOW TOM (BMBOUND) : cacheable WP
	 * - 4GB ->  HIGH TOM (BMBOUND_HI): cacheable
	 *
	 */

	tomlow = bmbound = sideband_read(B_UNIT, BMBOUND);
	printk(BIOS_SPEW, "Top of Low Used DRAM (BMBOUND): 0x%08x\n", bmbound);

	bmbound_hi = (u64)(sideband_read(B_UNIT, BMBOUND_HI)) << 4;
	printk(BIOS_SPEW, "Top of Upper Used DRAM (BMBOUND_HI): 0x%llx\n", bmbound_hi);

	bsmmrrl = sideband_read(B_UNIT, BSMMRRL) << 20;
	bsmmrrh = ((sideband_read(B_UNIT, BSMMRRH) + 1) << 20) - 1;
	if (bsmmrrl) {
		tomlow = bsmmrrl;
		printk(BIOS_DEBUG, "SMM memory location: 0x%x  SMM memory size: 0x%x\n", bsmmrrl, (bsmmrrh - bsmmrrl + 1));
		printk(BIOS_DEBUG, "Subtracting %dM for SMM\n", (bmbound - bsmmrrl) >> 20);
	}
	tomlow -= FSP_RESERVE_MEMORY_SIZE;
	printk(BIOS_SPEW, "Available memory below 4GB: 0x%08x (%dM)\n", tomlow, tomlow >> 20);

	/* Report the memory regions. */
	ram_resource(dev, index++, 0, legacy_hole_base_k);
	ram_resource(dev, index++, legacy_hole_base_k + legacy_hole_size_k,
	     ((tomlow >> 10) - (legacy_hole_base_k + legacy_hole_size_k)));

	mmio_resource(dev, index++, tomlow >> 10, (bmbound - bsmmrrl) >> 10);

	if (bmbound_hi > 0x100000000) {
		ram_resource(dev, index++, 0x100000000 >> 10, (bmbound_hi - 0x100000000) >> 10 );
		printk(BIOS_INFO, "Available memory above 4GB: %lluM\n", (bmbound_hi - 0x100000000) >> 20);
	}

	index = add_fixed_resources(dev, index);
}

static void mc_read_resources(struct device *dev)
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

static void pci_domain_set_resources(struct device *dev)
{
	/*
	 * Assign memory resources for PCI devices
	 */
	mc_add_dram_resources(dev);

	assign_resources(dev->link_list);
}

static void mc_set_resources(struct device *dev)
{
	/* Call the normal set_resources */
	pci_dev_set_resources(dev);
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

static void northbridge_init(struct device *dev)
{
}

static void northbridge_enable(struct device *dev)
{
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem    = intel_set_subsystem,
};

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
};

static struct device_operations mc_ops = {
	.read_resources   = mc_read_resources,
	.set_resources    = mc_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init,
	.acpi_fill_ssdt_generator = northbridge_acpi_fill_ssdt_generator,
	.enable           = northbridge_enable,
	.scan_bus         = 0,
	.ops_pci          = &intel_pci_ops,
};

/*
 * The following entries are taken from Intel document number 510524, rev 1.6:
 * Rangeley SoC External Design Specification (EDS)
 * Section 10.3 PCI Configuration Space
 * Table 10-6. PCI Devices and Functions
 *
 * These are the Device ID values for the item at bus 0, device 0, function 0.
 */
static const unsigned short pci_device_ids[] = {
	0x1f00, 0x1f01, 0x1f02, 0x1f03,
	0x1f04, 0x1f05, 0x1f06, 0x1f07,
	0x1f08, 0x1f09, 0x1f0a, 0x1f0b,
	0x1f0c, 0x1f0d, 0x1f0e, 0x1f0f,
	0,			/* -- END OF LIST -- */
};

static const struct pci_driver mc_driver __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
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

struct chip_operations northbridge_intel_fsp_rangeley_ops = {
	CHIP_NAME("Intel Rangeley Northbridge")
	.enable_dev = enable_dev,
};
