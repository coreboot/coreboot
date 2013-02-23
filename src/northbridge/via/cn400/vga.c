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

/*
 * Note: Some of the VGA control registers are located on the memory
 * controller. Registers are set both in raminit.c and northbridge.c.
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <arch/interrupt.h>
#include "northbridge.h"
#include "cn400.h"
#include <x86emu/regs.h>

static int via_cn400_int15_handler(void)
{
	int res=0;
	printk(BIOS_DEBUG, "via_cn400_int15_handler\n");
	switch(X86_EAX & 0xffff) {
	case 0x5f19:
		break;
	case 0x5f18:
		X86_EAX=0x5f;
		X86_EBX=0x545; // MCLK = 133, 32M frame buffer, 256 M main memory
		X86_ECX=0x060;
		res=1;
		break;
	case 0x5f00:
		X86_EAX = 0x8600;
		break;
	case 0x5f01:
		X86_EAX = 0x5f;
		X86_ECX = (X86_ECX & 0xffffff00 ) | 2; // panel type =  2 = 1024 * 768
		res = 1;
		break;
	case 0x5f02:
		X86_EAX=0x5f;
		X86_EBX= (X86_EBX & 0xffff0000) | 2;
		X86_ECX= (X86_ECX & 0xffff0000) | 0x401;  // PAL + crt only
		X86_EDX= (X86_EDX & 0xffff0000) | 0;  // TV Layout - default
		res=1;
		break;
	case 0x5f0f:
		X86_EAX=0x860f;
		break;
        default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
				X86_EAX & 0xffff);
		break;
	}
	return res;
}

static void vga_init(device_t dev)
{
	u8 reg8;

	mainboard_interrupt_handlers(0x15, &via_cn400_int15_handler);

	/* Set memory rate to 200 MHz. */
	outb(0x3d, CRTM_INDEX);
	reg8 = inb(CRTM_DATA);
	reg8 &= 0x0f;
	reg8 |= (0x1 << 4);
	outb(0x3d, CRTM_INDEX);
	outb(reg8, CRTM_DATA);

	/* Set framebuffer size. */
	reg8 = (CONFIG_VIDEO_MB / 4);
	outb(0x39, SR_INDEX);
	outb(reg8, SR_DATA);

	pci_write_config8(dev, 0x04, 0x07);
	pci_write_config8(dev, 0x0d, 0x20);
	pci_write_config32(dev, 0x10, 0xf0000008);
	pci_write_config32(dev, 0x14, 0xf4000000);

	printk(BIOS_DEBUG, "Initializing VGA...\n");

	pci_dev_init(dev);

	/* It's not clear if these need to be programmed before or after
	 * the VGA BIOS runs. Try both, clean up later. */

	/* Set memory rate to 200 MHz (again). */
	outb(0x3d, CRTM_INDEX);
	reg8 = inb(CRTM_DATA);
	reg8 &= 0x0f;
	reg8 |= (0x1 << 4);
	outb(0x3d, CRTM_INDEX);
	outb(reg8, CRTM_DATA);

	/* Set framebuffer size (again). */
	reg8 = (CONFIG_VIDEO_MB / 4);
	outb(0x39, SR_INDEX);
	outb(reg8, SR_DATA);

#ifdef DEBUG_CN400
	int i, j;

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

static const struct device_operations vga_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = vga_init,
	.ops_pci          = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops    = &vga_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN400_VGA,
};
