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

#define RAM_4GB		(((u64)1) << 32)

static uint64_t uma_memory_base = 0;
static uint64_t uma_memory_size = 0;

/**
 * @file vx900/northbridge.c
 *
 * STATUS: Pretty good
 * One thing that needs to be thoroughly tested is the remap above 4G logic.
 * Unfortunately, while we cannot initialize odd ranks, our testing
 * possibilities are somewhat limited. A point of failure that is not covered is
 * when the amount of RAM and PCI config space added up exceeds 8GB. The
 * remapping mechanism will overflow, the effects of which are unknown.
 */

void do_hard_reset(void)
{
	outb((1 << 2) | (1 << 1), 0xcf9);
}

uint64_t get_uma_memory_base(void)
{
	printk(BIOS_DEBUG, "UMA base 0x%.8llx (%lluMB)\n", uma_memory_base,
		uma_memory_base >> 20);
	printk(BIOS_DEBUG, "UMA size 0x%.8llx (%lluMB)\n", uma_memory_size,
		uma_memory_size >> 20);
	return uma_memory_base;
}

static u64 vx900_get_top_of_ram(struct device *mcu)
{
	u16 reg16;
	/* The last valid DRAM address is computed by the MCU
	 * One issue might be if we have a hole in the rank mappings, so that
	 * virtual ranks are not mapped successively in the linear address space
	 * (Ex: rank 0 mapped 0-1G, rank 1 mapped 2G-3G)
	 * We don't do this awkward mapping in RAM init, so we don't worry about
	 * it here, but it is something to keep in mind if having RAM issues */
	reg16 = pci_read_config16(mcu, 0x88) & 0x07ff;
	return (u64) reg16 << 24;
}

/*
 * This guy is meant to go away, but for now, leave it in so that we can see
 * if the logic to remap RAM above 4G has errors.
 */
static void killme_debug_4g_remap_reg(u32 reg32)
{
	if (reg32 & (1 << 0))
		printk(BIOS_DEBUG, "Mem remapping enabled\n");
	u64 remapstart = (reg32 >> 2) & 0x3ff;
	u64 remapend = (reg32 >> 14) & 0x3ff;
	remapstart <<= 26;
	remapend <<= 26;
	printk(BIOS_DEBUG, "Remapstart %lld(MB)\n", remapstart >> 20);
	printk(BIOS_DEBUG, "Remapend   %lld(MB)\n", remapend >> 20);
}

/**
 * \brief Remap low memory colliding with PCI MMIO space, above 4G
 *
 * @param mcu The memory controller
 * @param tolm Top of low memory.
 *
 * @return The new top of memory.
 */
static u64 vx900_remap_above_4g(struct device *mcu, u32 tolm)
{
	size_t i;
	u8 reg8, start8, end8, start, end;
	u16 reg16;
	u32 reg32;
	u64 tor, newtor, chunk;

	/*
	 * The remapping mechanism works like this:
	 *
	 * - Choose the top of low memory.
	 *      This becomes the "remap from"
	 * - Choose a chunk above 4G where to remap.
	 *      This becomes "remap to"
	 * - Choose a chunk above 4G where to end the remapping.
	 *      This becomes "remap until"
	 *
	 * This remaps a "chunk" of memory where we want to.
	 *      sizeof(chunk) = until - to;
	 *
	 * Therefore the memory region from "from" to " from + sizeof(chunk)"
	 * becomes accessible at "to" to "until"
	 */
	if (tolm >= vx900_get_top_of_ram(mcu)) {
		printk(BIOS_DEBUG, "Nothing to remap\n");
		return 0;
	}

	/* This is how the Vendor BIOS. Keep it for comparison for now */
	killme_debug_4g_remap_reg(0x00180141);
	/* We can remap with a granularity of 64MB, so align tolm */
	tolm &= ~((64 * MiB) - 1);

	/* The "start remapping from where ?" register */
	reg16 = ((tolm >> 20) & 0xfff) << 4;
	pci_mod_config16(mcu, 0x84, 0xfff0, reg16);

	/* Find the chunk size */
	tor = vx900_get_top_of_ram(mcu);
	printk(BIOS_DEBUG, "Top of RAM %lldMB\n", tor >> 20);

	if (tor < RAM_4GB) {
		chunk = tor - tolm;
		newtor = RAM_4GB + chunk;
	} else {
		chunk = (RAM_4GB - tolm);
		newtor = tor + chunk;
	}
	printk(BIOS_DEBUG, "New top of RAM %lldMB\n", newtor >> 20);

	reg8 = tolm >> 26;
	/* Which rank does the PCI TOLM fall on? */
	for (i = 0; i < VX900_MAX_MEM_RANKS; i++) {
		end8 = pci_read_config8(mcu, 0x40 + i);
		if (reg8 > end8)
			continue;
		start8 = pci_read_config8(mcu, 0x48 + i);
		if (reg8 <= start8)
			continue;
		printk(BIOS_DEBUG, "Address %x falls on rank %zu\n", tolm, i);
		break;
	}

	for (; i < VX900_MAX_MEM_RANKS; i++) {
		start = pci_read_config8(mcu, 0x48 + i);
		end = pci_read_config8(mcu, 0x40 + i);

		if (end == 0) {
			printk(BIOS_DEBUG, "Huh? rank %zu empty?\n", i);
			continue;
		}

		if (end < (tolm >> 26)) {
			printk(BIOS_DEBUG, "Huh? rank %zu don't need remap?\n",
			       i);
			continue;
		}

		printk(BIOS_DEBUG, "Physical rank %u is mapped to\n"
		       "    Start address: 0x%.10llx (%dMB)\n"
		       "    End   address: 0x%.10llx (%dMB)\n",
		       (int)i,
		       ((u64) start << 26), (start << (26 - 20)),
		       ((u64) end << 26), (end << (26 - 20)));

		if (end < (RAM_4GB >> 26))
			end = (RAM_4GB >> 26);

		if (end >= (tolm >> 26))
			end += chunk >> 26;

		if (start > (tolm >> 26))
			start += chunk >> 26;

		pci_write_config8(mcu, 0x48 + i, start);
		pci_write_config8(mcu, 0x40 + i, end);

		printk(BIOS_DEBUG, "ReMapped Physical rank %u, to\n"
		       "    Start address: 0x%.10llx (%dMB)\n"
		       "    End   address: 0x%.10llx (%dMB)\n",
		       (int)i,
		       ((u64) start << 26), (start << (26 - 20)),
		       ((u64) end << 26), (end << (26 - 20)));
	}

	/* The "remap to where?" register */
	reg32 = ((MAX(tor, RAM_4GB) >> 26) & 0x3ff) << 2;
	/* The "remap until where?" register */
	reg32 |= ((newtor >> 26) & 0x3ff) << 14;
	/* Now enable the goodies */
	reg32 |= (1 << 0);
	pci_write_config32(mcu, 0xf8, reg32);
	printk(BIOS_DEBUG, "Wrote remap map %x\n", reg32);
	killme_debug_4g_remap_reg(reg32);

	printk(BIOS_DEBUG, "New top of memory is at %lldMB\n", newtor >> 20);
	return newtor;
}

static void vx900_set_resources(struct device *dev)
{
	u32 pci_tolm, tomk, vx900_tolm, full_tolmk, fbufk, tolmk;

	printk(BIOS_DEBUG, "========================================"
		    "========================================\n");
	printk(BIOS_DEBUG, "============= VX900 memory sizing & Co. "
		    "========================================\n");
	printk(BIOS_DEBUG, "========================================"
		    "========================================\n");

	int idx = 10;
	struct device *const mcu = dev_find_device(PCI_VENDOR_ID_VIA,
						PCI_DEVICE_ID_VIA_VX900_MEMCTRL,
						0);
	if (!mcu) {
		die("Something is terribly wrong.\n"
		    " We tried locating the MCU on the PCI bus, "
		    "but couldn't find it. Halting.\n");
	}

	/* How much low adrress space do we have? */
	pci_tolm = find_pci_tolm(dev->link_list);
	printk(BIOS_SPEW, "Found PCI tolm at           %.8x\n", pci_tolm);
	printk(BIOS_SPEW, "Found PCI tolm at           %dMB\n", pci_tolm >> 20);

	/* Figure out the total amount of RAM */
	tomk = vx900_get_top_of_ram(mcu) >> 10;
	printk(BIOS_SPEW, "Found top of memory at      %dMB\n", tomk >> 10);

	/* Do the same for top of low RAM */
	vx900_tolm = vx900_get_tolm();
	full_tolmk = vx900_tolm << (20 - 10);
	/* Remap above 4G if needed */
	full_tolmk = MIN(full_tolmk, pci_tolm >> 10);
	printk(BIOS_SPEW, "Found top of low memory at  %dMB\n",
	       full_tolmk >> 10);

	/* What about the framebuffer for the integrated GPU? */
	fbufk = vx900_get_chrome9hd_fb_size() << (20 - 10);
	printk(BIOS_SPEW, "Integrated graphics buffer: %dMB\n", fbufk >> 10);

	/* Can't use the framebuffer as system RAM, sorry */
	tolmk = MIN(full_tolmk, tomk);
	tolmk -= fbufk;
	ram_resource(dev, idx++, 0, 640);
	printk(BIOS_SPEW, "System RAM left:            %dMB\n", tolmk >> 10);
	/* FIXME: how can we avoid leaving this hole?
	 * Leave a hole for VGA, 0xa0000 - 0xc0000  ?? */
	/* TODO: VGA Memory hole can be disabled in SNMIC. Upper 64k of ROM seem
	 * to be always mapped to the top of 1M, but this can be overcome with
	 * some smart positive/subtractive resource decoding */
	ram_resource(dev, idx++, 768, (tolmk - 768));
	uma_memory_size = fbufk << 10;
	uma_memory_base = tolmk << 10;

	//uma_resource(dev, idx++, uma_memory_base>>10, uma_memory_size>>10);

	printk(BIOS_DEBUG, "UMA @ %lldMB + %lldMB\n", uma_memory_base >> 20,
	       uma_memory_size >> 20);
	/* FIXME: How do we handle remapping above 4G? */
	u64 tor = vx900_remap_above_4g(mcu, pci_tolm);
	if (tor)
		ram_resource(dev, idx++, RAM_4GB >> 10, (tor - RAM_4GB) >> 10);

	printk(BIOS_DEBUG, "======================================================\n");
	assign_resources(dev->link_list);
}

static void vx900_read_resources(struct device *dev)
{
	/* Our fixed resources start at 0 */
	int idx = 0;
	/* Reserve our ROM mapped space */
	struct resource *res;
	res = new_resource(dev, idx++);
	res->size = CONFIG_ROM_SIZE;
	res->base = 0xffffffff - (res->size - 1);
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Now do the same for our MMCONF
	 * We always run with MMCONF enabled. We need to access the extended
	 * config space when configuring PCI-Express links */
	mmconf_resource(dev, idx++);

	pci_domain_read_resources(dev);
}

static struct device_operations pci_domain_ops = {
	.read_resources = vx900_read_resources,
	.set_resources = vx900_set_resources,
	.enable_resources = NULL,
	.init = NULL,
	.scan_bus = pci_domain_scan_bus,
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

struct chip_operations northbridge_via_vx900_ops = {
	CHIP_NAME("VIA VX900 Chipset")
	    .enable_dev = enable_dev,
};
