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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* Note: Some of the VGA control registers are located on the memory controller.
   Registers are set both in raminit.c and northbridge.c */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>
#include "chip.h"
#include "northbridge.h"
#include "cn700.h"

void write_protect_vgabios(void)
{
//Don't bother for now
}

static void vga_init(device_t dev)
{
	u8 reg8;

	print_debug("Copying BOCHS Bios to 0xf000\n");
/* Copy the BOCHs BIOS from 0xFFFFFFFF - ROM_SIZE - BOCHs size (64k) to 0xf0000
   This is for compatibility with the VGA ROM's BIOS callbacks */
	memcpy(0xf0000, (0xFFFFFFFF - ROM_SIZE - 0x10000), 0x10000);

	printk_debug("Initializing VGA\n");
	
	pci_write_config8(dev, 0x3c, 0xb);

	/* Set memory rate to 200MHz */
	outb(0x3d, CRTM_INDEX);
	reg8 = inb(CRTM_DATA);
	reg8 &= 0x0f;
	reg8 |= (0x1 << 4);
	outb(0x3d, CRTM_INDEX);
	outb(reg8, CRTM_DATA);
	
	/* Set framebuffer size */
	reg8 = (CONFIG_VIDEO_MB / 4);
	outb(0x39, SR_INDEX);
	outb(reg8, SR_DATA);
	
	
	pci_write_config8(dev, 0x04, 0x07);
	pci_write_config8(dev, 0x0d, 0x20);
	pci_write_config32(dev,0x10, 0xf4000008);
	pci_write_config32(dev,0x14, 0xfb000000);
	pci_write_config8(dev, 0x3e, 0x02);
	pci_write_config8(dev, 0x3c, 0x0a);
	
	
	printk_debug("INSTALL REAL-MODE IDT\n");
	setup_realmode_idt();
	printk_debug("DO THE VGA BIOS\n");
	do_vgabios();
	/* VGA seems to work without this, but crash & burn with it */
	//printk_debug("Enable VGA console\n");
	//vga_enable_console();

	/* It's not clear if these need to be programmed before or after
	 * the VGA bios runs. Try both, clean up later */
	/* Set memory rate to 200MHz */
	outb(0x3d, CRTM_INDEX);
	reg8 = inb(CRTM_DATA);
	reg8 &= 0x0f;
	reg8 |= (0x1 << 4);
	outb(0x3d, CRTM_INDEX);
	outb(reg8, CRTM_DATA);
	
	/* Set framebuffer size (again) */
	reg8 = (CONFIG_VIDEO_MB / 4);
	outb(0x39, SR_INDEX);
	outb(reg8, SR_DATA);

	/* Clear the BOCHs Bios out of memory, so it doesn't confuse linux */
	memset(0xf0000, 0, 0x10000);
}

static void vga_read_resources(device_t dev)
{
	dev->rom_address = 0xfff80000;
	dev->on_mainboard=1;
	pci_dev_read_resources(dev);
}


static const struct device_operations vga_operations = {
	.read_resources   = vga_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = vga_init,
	.ops_pci          = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops = &vga_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN700_VGA,
};
