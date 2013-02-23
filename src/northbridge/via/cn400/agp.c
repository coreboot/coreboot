/*
 * This file is part of the coreboot project.
 *
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
#include "northbridge.h"
#include "cn400.h"

/* This is the main AGP device, and only one used when configured for AGP 2.0 */
static void agp_init(device_t dev)
{
	u32 reg32;
	u8 reg8;
	int i, j;

	/* Some of this may not be necessary (should be handled by the OS). */
	printk(BIOS_DEBUG, "Enabling AGP.\n");

	/* Allow R/W access to AGP registers. */
	pci_write_config8(dev, 0x4d, 0x05);

	/* Setup PCI latency timer. */
	pci_write_config8(dev, 0xd, 0x8);

	/* Write Secondary Vendor Ids */
	pci_write_config32(dev, 0x2C, 0xAA071106);

	/*
	 * Set to AGP 3.0 Mode, which should theoretically render the rest of
	 * the registers set here pointless.
	 */
	pci_write_config8(dev, 0x84, 0x1b);

	/* AGP Request Queue Size */
	pci_write_config8(dev, 0x4a, 0x1f);

	/*
	 * AGP Hardware Support (default 0xc4)
	 * 7: AGP SBA Enable (1 to Enable)
	 * 6: AGP Enable
	 * 5: Reserved
	 * 4: Fast Write Enable
	 * 3: AGP8X Mode Enable
	 * 2: AGP4X Mode Enable
	 * 1: AGP2X Mode Enable
	 * 0: AGP1X Mode Enable
	 */
	pci_write_config8(dev, 0x4b, 0xc4);

	/* Enable AGP Backdoor */
	pci_write_config8(dev, 0xb5, 0x03);

	/* Set aperture to 128 MB. */
	/* TODO: Use config option, explain how it works. */
	pci_write_config32(dev, 0x94, 0x00010f20);
	/* Set GART Table Base Address (31:12). */
	pci_write_config32(dev, 0x98, (0x37b20 << 12));
	/* Set AGP Aperture Base. */
	pci_write_config32(dev, 0x10, 0xe8000008);

	/* NMI/AGPBUSY# Function Select */
	pci_write_config8(dev, 0xbe, 0x80);

	/* AGP Misc Control 1 */
	pci_write_config8(dev, 0xc2, 0x40);

	/* Enable CPU/PMSTR GART Access and DBI function. */
	reg32 = pci_read_config8(dev, 0xbf);
	reg32 |= 0x8c;
	pci_write_config8(dev, 0xbf, reg32);

	/* Enable AGP Aperture. */
	pci_write_config32(dev, 0x90, 0x0180);

	/* AGP Control */
	pci_write_config8(dev, 0xbc, 0x25);
	pci_write_config8(dev, 0xbd, 0xd2);

	/*
	 * AGP Pad, driving strength, and delay control. All this should be
	 * constant, seeing as the VGA controller is onboard.
	 */
	pci_write_config8(dev, 0x40, 0xda);
	pci_write_config8(dev, 0x41, 0xca);
	pci_write_config8(dev, 0x42, 0x01);
	pci_write_config8(dev, 0x43, 0xca);
	pci_write_config8(dev, 0x44, 0x04);

	/* AGPC CKG Control */
	pci_write_config8(dev, 0xc0, 0x04);
	pci_write_config8(dev, 0xc1, 0x02);

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
}

static const struct device_operations agp_operations = {
	.read_resources   = cn400_noop,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = agp_init,
	.ops_pci          = 0,
};

static const struct pci_driver agp_driver __pci_driver = {
	.ops    = &agp_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN400_AGP,
};

static void agp_bridge_read_resources (device_t dev)
{
	struct resource *res;

	res = new_resource(dev, 1);
	res->base = 0xF0000000ULL;
	res->size = 0x06000000ULL;
	res->limit = 0xffffffffULL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
		     		IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	res = new_resource(dev, 2);
	res->base = 0xB000UL;
	res->size = 4096;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED |
		     		IORESOURCE_STORED | IORESOURCE_ASSIGNED;

}
/*
 * This is the AGP 3.0 "bridge" @Bus 0 Device 1 Func 0. When using AGP 3.0, the
 * config in this device takes presidence. We configure both just to be safe.
 */
static void agp_bridge_init(device_t dev)
{
	u8 reg8;
	int i, j;

	printk(BIOS_DEBUG, "Entering %s\n", __func__);

	pci_write_config16(dev, 0x4, 0x0107);

	/* Secondary Bus Number */
	pci_write_config8(dev, 0x19, 0x01);
	/* Subordinate Bus Number */
	pci_write_config8(dev, 0x1a, 0x01);

	/* I/O Base */
	pci_write_config8(dev, 0x1c, 0xf0);

	/* I/O Limit */
	pci_write_config8(dev, 0x1d, 0x00);

	/* Memory Base */
	pci_write_config16(dev, 0x20, 0xf400);

	/* Memory Limit */
	pci_write_config16(dev, 0x22, 0xf5f0);

	/* Prefetchable Memory Base */
	pci_write_config16(dev, 0x24, 0xf000);

	/* Prefetchable Memory Limit */
	pci_write_config16(dev, 0x26, 0xf3f0);

	/* Enable VGA Compatible Memory/IO Range */
	pci_write_config8(dev, 0x3e, 0x0e);

	/* AGP Bus Control */
	pci_write_config8(dev, 0x40, 0x83);
	pci_write_config8(dev, 0x41, 0xC7);
	pci_write_config8(dev, 0x42, 0x02);
	pci_write_config8(dev, 0x43, 0x44);
	pci_write_config8(dev, 0x44, 0x34);
	pci_write_config8(dev, 0x45, 0x72);

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

}

static const struct device_operations agp_bridge_operations = {
	.read_resources   = agp_bridge_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = agp_bridge_init,
	.scan_bus         = pci_scan_bridge,
	.ops_pci          = 0,
};

static const struct pci_driver agp_bridge_driver __pci_driver = {
	.ops    = &agp_bridge_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN400_BRIDGE,
};
