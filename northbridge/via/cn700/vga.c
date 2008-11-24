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

#include <console.h>
#include <io.h>
#include <types.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "cn700.h"

void write_protect_vgabios(void)
{
	/* Don't bother for now. */
#warning "CN700 VGA BIOS write protect needs to be completed"
}

static void vga_init(struct device *dev)
{
	u8 reg8;

	printk(BIOS_DEBUG, "Initializing VGA\n");

	/* Set memory rate to 200 MHz. */
	outb(0x3d, CRTM_INDEX);
	reg8 = inb(CRTM_DATA);
	reg8 &= 0x0f;
	reg8 |= (0x1 << 4);
	outb(0x3d, CRTM_INDEX);
	outb(reg8, CRTM_DATA);

	/* Set framebuffer size. */
	reg8 = (CONFIG_CN700_VIDEO_MB_32 / 4);
	outb(0x39, SR_INDEX);
	outb(reg8, SR_DATA);

	pci_write_config8(dev, 0x04, 0x07);
	pci_write_config8(dev, 0x0d, 0x20);
	/* TODO: IIRC these need to be fixed for different VGA memory sizes */
	pci_write_config32(dev, 0x10, 0xf4000008);
	pci_write_config32(dev, 0x14, 0xfb000000);
}

static void vga_read_resources(struct device *dev)
{
	dev->rom_address = (0xffffffff - (u32)(1024 * CONFIG_COREBOOT_ROMSIZE_KB - 1)); 
	dev->on_mainboard = 1;
	pci_dev_read_resources(dev);
}

struct device_operations cn700_vga = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_VIA,
				.device = PCI_DEVICE_ID_VIA_CN700_VGA}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= scan_static_bus,
	.phase4_read_resources		= vga_read_resources,
	.phase4_set_resources		= pci_set_resources,
	.phase5_enable_resources	= pci_dev_enable_resources,
	.phase6_init			= vga_init,
};
