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
#include <cbmem.h>
#include <cpu/cpu.h>
#include <arch/acpi.h>
#include "i945.h"

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

static void pci_domain_set_resources(struct device *dev)
{
	uint32_t pci_tolm, tseg_sizek;
	uint8_t tolud;
	uint16_t reg16;
	unsigned long long tomk, tomk_stolen;
	uint64_t uma_memory_base = 0, uma_memory_size = 0;
	uint64_t tseg_memory_base = 0, tseg_memory_size = 0;

	/* Can we find out how much memory we can use at most
	 * this way?
	 */
	pci_tolm = find_pci_tolm(dev->link_list);
	printk(BIOS_DEBUG, "pci_tolm: 0x%x\n", pci_tolm);

	printk(BIOS_SPEW, "Base of stolen memory: 0x%08x\n",
		    pci_read_config32(dev_find_slot(0, PCI_DEVFN(2, 0)), BSM));

	tolud = pci_read_config8(dev_find_slot(0, PCI_DEVFN(0, 0)), TOLUD);
	printk(BIOS_SPEW, "Top of Low Used DRAM: 0x%08x\n", tolud << 24);

	tomk = tolud << 14;
	tomk_stolen = tomk;

	/* Note: subtract IGD device and TSEG */
	reg16 = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0, 0)), GGC);
	if (!(reg16 & 2)) {
		printk(BIOS_DEBUG, "IGD decoded, subtracting ");
		int uma_size = decode_igd_memory_size((reg16 >> 4) & 7);

		printk(BIOS_DEBUG, "%dM UMA\n", uma_size >> 10);
		tomk_stolen -= uma_size;

		/* For reserving UMA memory in the memory map */
		uma_memory_base = tomk_stolen * 1024ULL;
		uma_memory_size = uma_size * 1024ULL;
	}

	tseg_sizek = decode_tseg_size(pci_read_config8(dev_find_slot(0,
					PCI_DEVFN(0, 0)), ESMRAMC)) >> 10;
	printk(BIOS_DEBUG, "TSEG decoded, subtracting %dM\n", tseg_sizek >> 10);
	tomk_stolen -= tseg_sizek;
	tseg_memory_base = tomk_stolen * 1024ULL;
	tseg_memory_size = tseg_sizek * 1024ULL;

	/* The following needs to be 2 lines, otherwise the second
	 * number is always 0
	 */
	printk(BIOS_INFO, "Available memory: %dK", (uint32_t)tomk_stolen);
	printk(BIOS_INFO, " (%dM)\n", (uint32_t)(tomk_stolen >> 10));

	/* Report the memory regions */
	ram_resource(dev, 3, 0, 640);
	ram_resource(dev, 4, 768, (tomk - 768));
	uma_resource(dev, 5, uma_memory_base >> 10, uma_memory_size >> 10);
	mmio_resource(dev, 6, tseg_memory_base >> 10, tseg_memory_size >> 10);

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

static void intel_set_subsystem(struct device *dev, unsigned int vendor,
				unsigned int device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem    = intel_set_subsystem,
};

static struct device_operations mc_ops = {
	.read_resources   = mc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
	.scan_bus         = 0,
	.ops_pci          = &intel_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	0x2770, /* desktop */
	0x27a0, 0x27ac, /* mobile */
	0 };

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
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
}

struct chip_operations northbridge_intel_i945_ops = {
	CHIP_NAME("Intel i945 Northbridge")
	.enable_dev = enable_dev,
};
