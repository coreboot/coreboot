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
#include <bitops.h>
#include <cpu/cpu.h>
#include <arch/interrupt.h>
#include "chip.h"
#include "northbridge.h"
#include "cn400.h"

static int via_cn400_int15_handler(struct eregs *regs)
{
	int res=-1;
	printk(BIOS_DEBUG, "via_cn400_int15_handler\n");
	switch(regs->eax & 0xffff) {
	case 0x5f19:
		break;
	case 0x5f18:
		regs->eax=0x5f;
		regs->ebx=0x545; // MCLK = 133, 32M frame buffer, 256 M main memory
		regs->ecx=0x060;
		res=0;
		break;
	case 0x5f00:
		regs->eax = 0x8600;
		break;
	case 0x5f01:
		regs->eax = 0x5f;
		regs->ecx = (regs->ecx & 0xffffff00 ) | 2; // panel type =  2 = 1024 * 768
		res = 0;
		break;
	case 0x5f02:
		regs->eax=0x5f;
		regs->ebx= (regs->ebx & 0xffff0000) | 2;
		regs->ecx= (regs->ecx & 0xffff0000) | 0x401;  // PAL + crt only
		regs->edx= (regs->edx & 0xffff0000) | 0;  // TV Layout - default
		res=0;
		break;
	case 0x5f0f:
		regs->eax=0x860f;
		break;
        default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
				regs->eax & 0xffff);
		break;
	}
	return res;
}

static void vga_init(device_t dev)
{
	u8 reg8;

	mainboard_interrupt_handlers(0x15, &via_cn400_int15_handler);

#undef OLD_BOCHS_METHOD
#ifdef OLD_BOCHS_METHOD
	u32 temp;
	// XXX We might need more bios hooks in the f segment, but
	// this way of copying the BOCHS BIOS does not work anymore.
	// As soon as someone verifies that CN400 can init VGA, the
	// code should be removed.
	temp = (0xffffffff - CONFIG_FALLBACK_SIZE - 0xffff);
	printk(BIOS_DEBUG, "Copying BOCHS BIOS from 0x%08X	to 0xf000\n", temp);
	/*
	 * Copy BOCHS BIOS from 4G-CONFIG_FALLBACK_SIZE-64k (in flash) to 0xf0000 (in RAM)
	 * This is for compatibility with the VGA ROM's BIOS callbacks.
	 */
	//memcpy(0xf0000, (0xffffffff - CONFIG_ROM_SIZE - 0xffff), 0x10000);
	memcpy((void *)0xf0000, (void *)temp, 0x10000);
#endif

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

#ifdef OLD_BOCHS_METHOD
	/* Clear the BOCHS BIOS out of memory, so it doesn't confuse Linux. */
	memset((void *)0xf0000, 0, 0x10000);
#endif

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
