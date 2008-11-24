/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Corey Osgood <corey.osgood@gmail.com>
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

#include <types.h>
#include <console.h>
#include <io.h>
#include <device/pci.h>
#include <device/pci_ids.h>
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

struct device_operations cn700_agp = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_VIA,
				.device = PCI_DEVICE_ID_VIA_CN700_AGP}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= 0,
	.phase4_read_resources		= pci_dev_read_resources,
	//.phase4_set_resources		= pci_set_resources,
	//.phase5_enable_resources	= pci_dev_enable_resources,
	.phase6_init			= agp_init,
};
