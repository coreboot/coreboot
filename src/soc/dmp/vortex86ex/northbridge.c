/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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
#include <cbmem.h>
#include <pc80/mc146818rtc.h>
#include "chip.h"
#include "northbridge.h"

#define SPI_BASE 0xfc00

static void northbridge_init(device_t dev)
{
	printk(BIOS_DEBUG, "Vortex86EX northbridge early init ...\n");
	// enable F0A/ECA/E8A/E4A/E0A/C4A/C0A shadow read/writable.
	pci_write_config32(dev, NB_REG_MAR, 0x3ff000f0);
	// enable C0000h - C3FFFh/C4000h - C7FFF can be in L1 cache selection.
	pci_write_config32(dev, NB_REG_HOST_CTL, (1 << 18) | (1 << 19));
	// Set SPI register base.
	pci_write_config16(dev, NB_REG_SPI_BASE, SPI_BASE | 1);
}

static struct device_operations northbridge_operations = {
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init
};

static const struct pci_driver northbridge_driver_6025 __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_RDC,
	.device = 0x6025,	/* EX CPU N/B ID */
};

/* Set CMOS register 15h/16h/17h/18h for base/extended
 * memory size. */
static void set_cmos_memory_size(unsigned long sizek)
{
	unsigned long ext_mem_size;
	u8 ext_mem_size_hb, ext_mem_size_lb;
	/* calculate memory size between 1M - 65M. */
	ext_mem_size = sizek - 1024;
	if (ext_mem_size > 65535)
		ext_mem_size = 65535;
	ext_mem_size_hb = (u8) (ext_mem_size >> 8);
	ext_mem_size_lb = (u8) (ext_mem_size & 0xff);
	/* Base memory is always 640K. */
	cmos_write(0x80, 0x15);
	cmos_write(0x02, 0x16);
	/* Write extended memory size. */
	cmos_write(ext_mem_size_lb, 0x17);
	cmos_write(ext_mem_size_hb, 0x18);
	/* register 0x30(48) is RTC_BOOT_BYTE for coreboot,
	 * don't touch it. */
}

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	uint32_t pci_tolm;

	printk(BIOS_SPEW, "Entering vortex86ex pci_domain_set_resources.\n");

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev->link_list->children;
	if (mc_dev) {
		unsigned long tomk, tolmk;
		int idx;
		int ss;
		/* Get DDRII size setting from northbridge register. */
		/* SS = 0 for 2MB, 1 for 4MB, 2 for 8MB, 3 for 16MB ... */
		ss = pci_read_config16(mc_dev, 0x6c);
		ss = ((ss >> 8) & 0xf);
		tomk = (2 * 1024) << ss;
		printk(BIOS_DEBUG, "I would set RAM size to %ld Mbytes\n", (tomk >> 10));
		/* Compute the top of Low memory */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk)
			/* The PCI hole does does not overlap the memory.
			 */
			tolmk = tomk;

		set_late_cbmem_top(tolmk * 1024);

		/* Report the memory regions */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);	/* first 640k */
		ram_resource(dev, idx++, 768, tolmk - 768);	/* leave a hole for vga */
		set_cmos_memory_size(tolmk);
	}
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci_bus      = pci_bus_default_ops,
};

static void enable_dev(struct device *dev)
{
	printk(BIOS_SPEW, "In vortex86ex enable_dev for device %s.\n", dev_path(dev));

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
}

struct chip_operations northbridge_dmp_vortex86ex_ops = {
	CHIP_NAME("DMP Vortex86EX Northbridge")
	.enable_dev = enable_dev,
};
