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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
#include <cpu/cpu.h>
#include "northbridge.h"
#include "cn700.h"

static void memctrl_init(device_t dev)
{
	device_t vlink_dev;
	u16 reg16;
	u8 ranks, pagec, paged, pagee, pagef, shadowreg;

	/* Set up the VGA framebuffer size. */
	reg16 = (log2(CONFIG_VIDEO_MB) << 12) | (1 << 15);
	pci_write_config16(dev, 0xa0, reg16);

	/* Set up VGA timers. */
	pci_write_config8(dev, 0xa2, 0x44);

	for (ranks = 0x4b; ranks >= 0x48; ranks--) {
		if (pci_read_config8(dev, ranks)) {
			ranks -= 0x48;
			break;
		}
	}
	if (ranks == 0x47)
		ranks = 0x00;
	reg16 = 0xaae0;
	reg16 |= ranks;
	/* GMINT Misc. FrameBuffer rank */
	pci_write_config16(dev, 0xb0, reg16);
	/* AGPCINT Misc. */
	pci_write_config8(dev, 0xb8, 0x08);

	/* Shadow RAM */
	pagec = 0xff, paged = 0xff, pagee = 0xff, pagef = 0x30;
	/* PAGE C, D, E are all read write enable */
	pci_write_config8(dev, 0x80, pagec);
	pci_write_config8(dev, 0x81, paged);
	pci_write_config8(dev, 0x82, pagee);
	/* PAGE F are read/writable */
	shadowreg = pci_read_config8(dev, 0x83);
	shadowreg |= pagef;
	pci_write_config8(dev, 0x83, shadowreg);
	/* vlink mirror */
	vlink_dev = dev_find_device(PCI_VENDOR_ID_VIA,
				    PCI_DEVICE_ID_VIA_CN700_VLINK, 0);
	if (vlink_dev) {
		pci_write_config8(vlink_dev, 0x61, pagec);
		pci_write_config8(vlink_dev, 0x62, paged);
		pci_write_config8(vlink_dev, 0x64, pagee);

		shadowreg = pci_read_config8(vlink_dev, 0x63);
		shadowreg |= pagef;
		pci_write_config8(vlink_dev, 0x63, shadowreg);
	}
}

static const struct device_operations memctrl_operations = {
	.read_resources = cn700_noop,
	.init           = memctrl_init,
};

static const struct pci_driver memctrl_driver __pci_driver = {
	.ops    = &memctrl_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN700_MEMCTRL,
};

#if CONFIG_WRITE_HIGH_TABLES
#include <cbmem.h>
#endif

static void pci_domain_set_resources(device_t dev)
{
	/* The order is important to find the correct RAM size. */
	static const u8 ramregs[] = { 0x43, 0x42, 0x41, 0x40 };
	device_t mc_dev;
	u32 pci_tolm;

	printk(BIOS_SPEW, "Entering cn700 pci_domain_set_resources.\n");

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev_find_device(PCI_VENDOR_ID_VIA,
				 PCI_DEVICE_ID_VIA_CN700_MEMCTRL, 0);

	if (mc_dev) {
		unsigned long tomk, tolmk;
		unsigned char rambits;
		int i, idx;

		/*
		 * Once the register value is not zero, the RAM size is
		 * this register's value multiply 64 * 1024 * 1024.
		 */
		for (rambits = 0, i = 0; i < ARRAY_SIZE(ramregs); i++) {
			rambits = pci_read_config8(mc_dev, ramregs[i]);
			if (rambits != 0)
				break;
		}

		tomk = rambits * 64 * 1024;
		printk(BIOS_DEBUG, "tomk is 0x%lx\n", tomk);
		/* Compute the Top Of Low Memory (TOLM), in Kb. */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does does not overlap the memory. */
			tolmk = tomk;
		}

#if CONFIG_WRITE_HIGH_TABLES
		high_tables_base = ((tolmk - CONFIG_VIDEO_MB * 1024) * 1024) - HIGH_MEMORY_SIZE;
		high_tables_size = HIGH_MEMORY_SIZE;
		printk(BIOS_DEBUG, "tom: %lx, high_tables_base: %llx, high_tables_size: %llx\n",
						tomk*1024, high_tables_base, high_tables_size);
#endif

		/* Report the memory regions. */
		idx = 10;
		/* TODO: Hole needed? */
		ram_resource(dev, idx++, 0, 640);	/* First 640k */
		/* Leave a hole for VGA, 0xa0000 - 0xc0000 */
		ram_resource(dev, idx++, 768,
			     (tolmk - 768 - CONFIG_VIDEO_MB * 1024));
	}
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
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
	printk(BIOS_SPEW, "In cn700 enable_dev for device %s.\n", dev_path(dev));

	/* Set the operations if it is a special bus type. */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		dev->ops = &pci_domain_ops;
		pci_set_method(dev);
	} else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_via_cn700_ops = {
	CHIP_NAME("VIA CN700 Northbridge")
	.enable_dev = enable_dev,
};
