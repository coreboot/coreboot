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
#include <lib.h>

static void vx900_set_resources(device_t dev)
{
	print_debug("========================================"
		    "========================================\n" );
	print_debug("============= VX900 memory sizing & Co. "
		    "========================================\n" );
	print_debug("========================================"
		    "========================================\n" );


	int idx = 0;
	const device_t mcu = dev_find_device(PCI_VENDOR_ID_VIA,
		PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	if(!mcu) {
		die("Something is terribly wrong.\n"
		    " We tried locating the MCU on the PCI bus, "
		    "but couldn't find it. Halting.\n");
	}

	u32 pci_tolm = find_pci_tolm(dev->link_list);
	printk(BIOS_SPEW, "Found PCI tolm at           %.8x\n", pci_tolm);
	printk(BIOS_SPEW, "Found PCI tolm at           %dMB\n", pci_tolm>>20);

	/* The last valid DRAM address is computed by the MCU
	 * One issue might be if we have a hole in the rank mappings, so that
	 * virtual ranks are not mapped successively in the linear address space
	 * (Ex: rank 0 mapped 0-1G, rank 1 mapped 2G-3G)
	 * We don't do this awkward mapping in RAM init, so we don't worry about
	 * it here, but it is something to keep in mind if having RAM issues */
	u32 vx900_tom = pci_read_config16(mcu, 0x88) & 0x07ff;
	const u32 tomk = vx900_tom << (24-10);
	printk(BIOS_SPEW, "Found top of memory at      %dMB\n", tomk>>10);

	/* Do the same for top of low RAM */
	u32 vx900_tolm = (pci_read_config16(mcu, 0x84) & 0xfff0) >> 4;
	const u32 full_tolmk = vx900_tolm << (20-10);
	printk(BIOS_SPEW, "Found top of low memory at  %dMB\n", full_tolmk>>10);

	/* What about the framebuffer for the integrated GPU? */
	u32 fbufk = chrome9hd_fb_size() >> 10;
	printk(BIOS_SPEW, "Integrated graphics buffer: %dMB\n", fbufk>>10);

	/* Can't use the framebuffer as system RAM, sorry */
	u32 tolmk = full_tolmk - fbufk;
	ram_resource(dev, idx++, 0, 640);
	printk(BIOS_SPEW, "System ram left:            %dMB\n", tolmk>>10);
	/* FIXME: how can we avoid leaving this hole?
	 * Leave a hole for VGA, 0xa0000 - 0xc0000  ?? */
	/* Unfortunately, the range 0xE0000-0xEFFFF is always mapped to the ROM.
	 * We've had some wierd things (need to reflash the ROM to get coreboot
	 * running again) go on
	 */
	ram_resource(dev, idx++, 1024, (tolmk - 1024));
	uma_memory_size = fbufk << 10;
	uma_memory_base = tolmk << 10;

	struct resource *res;
	res = new_resource(dev, idx++);
	res->base = 0xfff80000;
	res->size = 512<<10;
	//res->align = log2(res->size);
	//res->gran = log2(res->size);
	//res->limit = 0xffffffff;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_ASSIGNED |
		     IORESOURCE_STORED;

	#if CONFIG_WRITE_HIGH_TABLES
	/* Leave some space for ACPI, PIRQ and MP tables */
	high_tables_base = (tolmk<<10) - HIGH_MEMORY_SIZE;
	high_tables_size = HIGH_MEMORY_SIZE;
	printk(BIOS_DEBUG, "high_tables_base: %08llx, size %lld\n",
	       high_tables_base, high_tables_size);
	#endif
	print_debug("======================================================\n");
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
	.read_resources   = pci_domain_read_resources,
	.set_resources    = vx900_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
#if CONFIG_MMCONF_SUPPORT_DEFAULT
	.ops_pci_bus	  = &pci_ops_mmconf,
#else
	.ops_pci_bus	  = &pci_cf8_conf1,
#endif
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

