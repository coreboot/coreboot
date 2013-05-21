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
#include <reset.h>
#include <string.h>

void hard_reset(void)
{
	outb((1 << 2) | (1 << 1), 0xcf9);
}

static void vx900_set_resources(device_t dev)
{
	print_debug("========================================"
		    "========================================\n" );
	print_debug("============= VX900 memory sizing & Co. "
		    "========================================\n" );
	print_debug("========================================"
		    "========================================\n" );

	int idx = 10;
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
	u32 full_tolmk = vx900_tolm << (20-10);
	/* FIXME: Remap above 4G */
	full_tolmk = min(full_tolmk, pci_tolm >> 10);
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
	/* TODO: VGA Memory hole can be disabled in SNMIC. Upper 64k of ROM seem
	 * to be always mapped to the top of 1M, but this can be overcome with
	 * some smart positive/subtractive resource decoding */
	ram_resource(dev, idx++, 768, (tolmk - 768));
	uma_memory_size = fbufk << 10;
	uma_memory_base = tolmk << 10;

	/* Leave some space for ACPI, PIRQ and MP tables */
	high_tables_base = (tolmk<<10) - HIGH_MEMORY_SIZE;
	high_tables_size = HIGH_MEMORY_SIZE;
	printk(BIOS_DEBUG, "high_tables_base: %08llx, size %lld\n",
	       high_tables_base, high_tables_size);
	/* Because of the video framebuffer, the high tables may be in a
	 * different location than in romstage, so we need to copy them over */
/*	void* old_tables = (void*)((full_tolmk<<10) - HIGH_MEMORY_SIZE);
	void* new_tables = (void*)((u32)high_tables_base);
	printk(BIOS_DEBUG, "Moving CBMEM from %p to %p\n",
			   old_tables, new_tables);
	memcpy(new_tables, old_tables, HIGH_MEMORY_SIZE);
	cbmem_reinit(high_tables_base);*/

	print_debug("======================================================\n");
	assign_resources(dev->link_list);
}

static void vx900_read_resources(device_t dev)
{
	/* Our fixed resources start at 0 */
	int idx = 0;
	/* Reserve our ROM mapped space */
	struct resource *res;
	res = new_resource(dev, idx++);
	res->size = 512<<10;
	res->base = 0xffffffff - (res->size -1);
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Now do the same for our MMCONF */
	res = new_resource(dev, idx++);
	res->size = 256<<20;
	res->base = CONFIG_MMCONF_BASE_ADDRESS;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	pci_domain_read_resources(dev);
}
static struct device_operations pci_domain_ops = {
	.read_resources   = vx900_read_resources,
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

static void enable_dev(device_t dev)
{
	print_debug("Ve ar zelectin ar zomain\n");
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_via_vx900_ops = {
	CHIP_NAME("VIA VX900 Chipset")
	.enable_dev = enable_dev,
};

