/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vx900.h"
#include "chip.h"

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/cpu.h>
#include <cbmem.h>

static void vx900_read_resources(device_t dev)
{
	print_debug("========= VX900 memory sizing & Co. ==========\n");
	int idx = 0;
	/* FIXME: just a quick memory sizing. fix me later */
	unsigned long tolm = (1<<29);
	ram_resource(dev, idx++, 0, 640);
	/* FIXME: Leave a hole for SMRAM, 0xa0000 - 0xc0000 ?? */
	ram_resource(dev, idx++, 768, ((tolm >> 10) - 768));
	#if CONFIG_WRITE_HIGH_TABLES
	/* Leave some space for ACPI, PIRQ and MP tables */
	high_tables_base = tolm - HIGH_MEMORY_SIZE;
	high_tables_size = HIGH_MEMORY_SIZE;
	printk(BIOS_DEBUG, "high_tables_base: %08llx, size %lld\n",
	       high_tables_base, high_tables_size);
	#endif
	print_debug("==============================================\n");
}

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
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

static struct device_operations pci_domain_ops = {
	.read_resources   = vx900_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
};

static void enable_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_via_vx900_ops = {
	CHIP_NAME("VIA VX900 Chipset")
	.enable_dev = enable_dev,
};

