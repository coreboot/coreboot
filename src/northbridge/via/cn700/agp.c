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
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "northbridge.h"
#include "cn700.h"

/* This is the main AGP device, and only one used when configured for AGP 2.0 */
static void agp_init(struct device *dev)
{
	u32 reg32;

	/* Some of this may not be necessary (should be handled by the OS). */
	printk(BIOS_DEBUG, "Enabling AGP.\n");

	/* Allow R/W access to AGP registers. */
	pci_write_config8(dev, 0x4d, 0x15);

	/* Setup PCI latency timer. */
	pci_write_config8(dev, 0xd, 0x8);

	/*
	 * Set to AGP 3.0 Mode, which should theoretically render the rest of
	 * the registers set here pointless.
	 */
	pci_write_config8(dev, 0x84, 0xb);

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

	/* Set aperture to 32 MB. */
	/* TODO: Use config option, explain how it works. */
	pci_write_config32(dev, 0x94, 0x00010f38);
	/* Set GART Table Base Address (31:12). */
	pci_write_config32(dev, 0x98, (0x1558 << 12));
	/* Set AGP Aperture Base. */
	pci_write_config32(dev, 0x10, 0xf8000008);

	/* Enable CPU/PMSTR GART Access. */
	reg32 = pci_read_config8(dev, 0xbf);
	reg32 |= 0x80;
	pci_write_config8(dev, 0xbf, reg32);

	/* Enable AGP Aperture. */
	reg32 = pci_read_config32(dev, 0x94);
	reg32 |= (3 << 7);
	pci_write_config32(dev, 0x90, reg32);

	/* AGP Control */
	pci_write_config8(dev, 0xbc, 0x21);
	pci_write_config8(dev, 0xbd, 0xd2);

	/*
	 * AGP Pad, driving strength, and delay control. All this should be
	 * constant, seeing as the VGA controller is onboard.
	 */
	pci_write_config8(dev, 0x40, 0xc7);
	pci_write_config8(dev, 0x41, 0xdb);
	pci_write_config8(dev, 0x42, 0x10);
	pci_write_config8(dev, 0x43, 0xdb);
	pci_write_config8(dev, 0x44, 0x24);

	/* AGPC CKG Control */
	pci_write_config8(dev, 0xc0, 0x02);
	pci_write_config8(dev, 0xc1, 0x02);
}

static const struct device_operations agp_operations = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = agp_init,
	.ops_pci          = 0,
};

static const struct pci_driver agp_driver __pci_driver = {
	.ops    = &agp_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN700_AGP,
};

/*
 * This is the AGP 3.0 "bridge" @Bus 0 Device 1 Func 0. When using AGP 3.0, the
 * config in this device takes presidence. We configure both just to be safe.
 */
static void agp_bridge_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Setting up AGP bridge device\n");

	pci_write_config16(dev, 0x4, 0x0007);

	/* Secondary Bus Number */
	pci_write_config8(dev, 0x19, 0x01);
	/* Subordinate Bus Number */
	pci_write_config8(dev, 0x1a, 0x01);
	/* I/O Base */
	pci_write_config8(dev, 0x1c, 0xd0);
	/* I/O Limit */
	pci_write_config8(dev, 0x1d, 0xd0);

	/* Memory Base */
	pci_write_config16(dev, 0x20, 0xfb00);
	/* Memory Limit */
	pci_write_config16(dev, 0x22, 0xfcf0);
	/* Prefetchable Memory Base */
	pci_write_config16(dev, 0x24, 0xf400);
	/* Prefetchable Memory Limit */
	pci_write_config16(dev, 0x26, 0xf7f0);
	/* Enable VGA Compatible Memory/IO Range */
	pci_write_config8(dev, 0x3e, 0x08);

	/* Second PCI Bus Control (see datasheet) */
	pci_write_config8(dev, 0x40, 0x83);
	pci_write_config8(dev, 0x41, 0x43);
	pci_write_config8(dev, 0x42, 0xe2);
	pci_write_config8(dev, 0x43, 0x44);
	pci_write_config8(dev, 0x44, 0x34);
	pci_write_config8(dev, 0x45, 0x72);
}

static void agp_bridge_read_resources(struct device *dev)
{
	struct resource *resource;

	resource = new_resource(dev, 0);
	if (resource) {
		resource->base	= 0;
		resource->size	= 0;
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO | IORESOURCE_BRIDGE;
	}

	resource = new_resource(dev, 1);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->limit = 0xfffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
		resource->flags |= IORESOURCE_BRIDGE;
	}

	resource = new_resource(dev, 2);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->limit = 0xffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_BRIDGE;
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
	.device = PCI_DEVICE_ID_VIA_CN700_BRIDGE,
};
