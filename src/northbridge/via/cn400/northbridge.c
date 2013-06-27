/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 VIA Technologies, Inc.
 * (Written by Aaron Lwe <aaron.lwe@gmail.com> for VIA)
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <lib.h>
#include <cbmem.h>
#include <cpu/cpu.h>
#include "northbridge.h"
#include "cn400.h"

static void memctrl_init(device_t dev)
{
	device_t vlink_dev;
	u16 reg16;
	u8 ranks, pagec, paged, pagee, pagef, shadowreg, reg8;
	int i, j;

	printk(BIOS_SPEW, "Entering cn400 memctrl_init.\n");
	/* vlink mirror */
	vlink_dev = dev_find_device(PCI_VENDOR_ID_VIA,
				    PCI_DEVICE_ID_VIA_CN400_VLINK, 0);

	/* Setup Low Memory Top */
	/* 0x47 == HA(32:25)    */
	/* 0x84/85 == HA(31:20) << 4 | DRAM Granularity */
	ranks = pci_read_config8(dev, 0x47);
	reg16 = (((u16)(ranks - 1) << 9) & 0xFFF0) | 0x01F0;

	pci_write_config16(dev, 0x84, reg16);
	printk(BIOS_SPEW, "Low Top Address = 0x%04X\n", reg16);

	/* Set up the VGA framebuffer size and Base Address   */
	/* Note dependencies between agp.c and vga.c and here */
	reg16 = (log2(CONFIG_VIDEO_MB) << 12) | (1 << 15) | 0xF00;
	pci_write_config16(dev, 0xa0, reg16);


	for (ranks = 0x4b; ranks >= 0x48; ranks--) {
		if (pci_read_config8(dev, ranks)) {
			ranks -= 0x48;
			break;
		}
	}
	if (ranks == 0x47)
		ranks = 0x00;
	reg16 = 0xaaf0;
	reg16 |= ranks;
	/* GMINT Misc. FrameBuffer rank */
	pci_write_config16(dev, 0xb0, reg16);
	/* AGPCINT Misc. */
	pci_write_config8(dev, 0xb8, 0x08);

	/* Arbritation Counters */
	pci_write_config8(dev, 0xb2, 0xaa);

	/* Write FIFO Setup */
	pci_write_config8(dev, 0xb3, 0x5a);

	/* Graphics control optimisation */
	pci_write_config8(dev, 0xb4, 0x0f);

	/* Shadow RAM */
	pagec = 0xff, paged = 0xff, pagee = 0xff, pagef = 0x30;
	/* PAGE C, D, E are all read write enable */
	pci_write_config8(dev, 0x80, pagec);
	pci_write_config8(dev, 0x81, paged);
	pci_write_config8(dev, 0x83, pagee);
	/* PAGE F are read/writable */
	shadowreg = pci_read_config8(dev, 0x82);
	shadowreg |= pagef;
	pci_write_config8(dev, 0x82, shadowreg);
		pci_write_config8(vlink_dev, 0x61, pagec);
		pci_write_config8(vlink_dev, 0x62, paged);
		pci_write_config8(vlink_dev, 0x64, pagee);

		shadowreg = pci_read_config8(vlink_dev, 0x63);
		shadowreg |= pagef;
		pci_write_config8(vlink_dev, 0x63, shadowreg);

	/* Activate VGA Frame Buffer */

	reg8 = pci_read_config8(dev, 0xA0);
	reg8 |= 0x01;
	pci_write_config8(dev, 0xA0, reg8);

#ifdef DEBUG_CN400
	printk(BIOS_SPEW, "%s PCI Header Regs::\n", dev_path(dev));

	for (i = 0 ; i < 16; i++)
	{
		printk(BIOS_SPEW, "%02X: ", i*16);
		for (j = 0; j < 16; j++)
		{
			reg8 = pci_read_config8(dev, j+(i*16));
			printk(BIOS_SPEW, "%02X ", reg8);
		}
		printk(BIOS_SPEW, "\n");
	}
#endif
	printk(BIOS_SPEW, "Leaving cn400 %s.\n", __func__);
}

static const struct device_operations memctrl_operations = {
	.read_resources = cn400_noop,
	.set_resources    = cn400_noop,
	.enable_resources = cn400_noop,
	.init           = memctrl_init,
	.ops_pci          = 0,
};

static const struct pci_driver memctrl_driver __pci_driver = {
	.ops    = &memctrl_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN400_MEMCTRL,
};

static void cn400_domain_read_resources(device_t dev)
{
	struct resource *resource;

	printk(BIOS_SPEW, "Entering %s.\n", __func__);

	/* Initialize the system wide I/O space constraints. */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->limit = 0xffffUL;
	resource->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
	    IORESOURCE_ASSIGNED;

	/* Initialize the system wide memory resources constraints. */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	resource->limit = 0xffffffffULL;
	resource->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
	    IORESOURCE_ASSIGNED;

	printk(BIOS_SPEW, "Leaving %s.\n", __func__);
}

#ifdef UNUSED_CODE
static void ram_reservation(device_t dev, unsigned long index,
			 unsigned long base, unsigned long size)
{
	struct resource *res;

	printk(BIOS_SPEW, "Configuring Via C3 LAPIC Fixed Resource\n");
	/* Fixed LAPIC resource */
	res = new_resource(dev, 1);
	res->base = (resource_t) base;
	res->size = size;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}
#endif

static void cn400_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	u32 pci_tolm;

	printk(BIOS_SPEW, "Entering %s.\n", __func__);

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev_find_device(PCI_VENDOR_ID_VIA,
				 PCI_DEVICE_ID_VIA_CN400_MEMCTRL, 0);

	if (mc_dev) {
		unsigned long tomk, tolmk;
		unsigned char rambits;
		int idx;

		rambits = pci_read_config8(mc_dev, 0x47);
		tomk = rambits * 32 * 1024;
		/* Compute the Top Of Low Memory (TOLM), in Kb. */
		tolmk = pci_tolm >> 10;
		printk(BIOS_SPEW, "tomk is 0x%lx, tolmk is 0x%08lX\n", tomk, tolmk);
		if (tolmk >= tomk) {
			/* The PCI hole does does not overlap the memory. */
			tolmk = tomk;
		}

		/* Locate the High Tables at the Top of Low Memory below the Video RAM */
		set_top_of_ram((tolmk - (CONFIG_VIDEO_MB *1024)) * 1024);

		/* Report the memory regions. */
		idx = 10;
		/* TODO: Hole needed? */
		ram_resource(dev, idx++, 0, 640);	/* First 640k */
		/* Leave a hole for VGA, 0xa0000 - 0xc0000 */
		ram_resource(dev, idx++, 768,
			     (tolmk - 768 - CONFIG_VIDEO_MB * 1024));
	}
	assign_resources(dev->link_list);

	printk(BIOS_SPEW, "Leaving %s.\n", __func__);
}

static unsigned int cn400_domain_scan_bus(device_t dev, unsigned int max)
{
	printk(BIOS_DEBUG, "Entering %s.\n", __func__);

	max = pci_scan_bus(dev->link_list, PCI_DEVFN(0, 0), 0xff, max);
	return max;
}

static struct device_operations pci_domain_ops = {
	.read_resources   = cn400_domain_read_resources,
	.set_resources    = cn400_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = cn400_domain_scan_bus,
	.ops_pci_bus      = pci_bus_default_ops,
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

static void enable_dev(struct device *dev)
{
	printk(BIOS_SPEW, "CN400: enable_dev for device %s.\n", dev_path(dev));

	/* Set the operations if it is a special bus type. */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_via_cn400_ops = {
	CHIP_NAME("VIA CN400 Northbridge")
	.enable_dev = enable_dev,
};
