/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <arch/interrupt.h>
#include "northbridge.h"

/* PCI Domain 1 Device 0 Function 0 */

#define SR_INDEX	0x3c4
#define SR_DATA		0x3c5
#define CRTM_INDEX	0x3b4
#define CRTM_DATA	0x3b5
#define CRTC_INDEX	0x3d4
#define CRTC_DATA	0x3d5

/* !!FIXME!! These were CONFIG_ options.  Fix it in uma_ram_setting.c too. */
#define VIACONFIG_VGA_PCI_10 0xf8000008
#define VIACONFIG_VGA_PCI_14 0xfc000000

static int via_vx800_int15_handler(struct eregs *regs)
{
	int res=-1;
	printk(BIOS_DEBUG, "via_vx800_int15_handler\n");
	switch(regs->eax & 0xffff) {
	case 0x5f19:
		regs->eax=0x5f;
		regs->ecx=0x03;
		res=0;
		break;
	case 0x5f18:
	{
		/*
		 * BL Bit[7:4]
		 * Memory Data Rate
		 * 0000: 66MHz
		 * 0001: 100MHz
		 * 0010: 133MHz
		 * 0011: 200MHz ( DDR200 )
		 * 0100: 266MHz ( DDR266 )
		 * 0101: 333MHz ( DDR333 )
		 * 0110: 400MHz ( DDR400 )
		 * 0111: 533MHz ( DDR I/II 533
		 * 1000: 667MHz ( DDR I/II 667)
		 * Bit[3:0]
		 * N:  Frame Buffer Size 2^N  MB
		 */
		u8 i;
		device_t dev;
		dev = dev_find_slot(0, PCI_DEVFN(0, 3));
		i = pci_read_config8(dev, 0xa1);
		i = (i & 0x70);
		i = i >> 4;
		if (i == 0) {
			regs->eax = 0x00;	//not support 5f18
			break;
		}
		i = i + 2;
		regs->ebx = (u32) i;
		i = pci_read_config8(dev, 0x90);
		i = (i & 0x07);
		i = i + 3;
		i = i << 4;
		regs->ebx = regs->ebx + ((u32) i);
		regs->eax = 0x5f;
		res = 0;
		break;
	}
	case 0x5f00:
		regs->eax = 0x005f;
		res = 0;
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
		regs->eax = 0x005f;
		res = 0;
		break;
        default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
				regs->eax & 0xffff);
		regs->eax = 0;
		break;
	}
	return res;
}

#ifdef UNUSED_CODE
static void write_protect_vgabios(void)
{
	device_t dev;

	printk(BIOS_INFO, "write_protect_vgabios\n");
	/* there are two possible devices. Just do both. */
	dev = dev_find_device(PCI_VENDOR_ID_VIA,
			      PCI_DEVICE_ID_VIA_VX855_MEMCTRL, 0);
	if (dev)
		pci_write_config8(dev, 0x80, 0xff);
	/*vx855 no th 0x61 reg */
	/*dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VX855_VLINK, 0);
	   //if(dev)
	   //   pci_write_config8(dev, 0x61, 0xff); */
}
#endif

static void vga_enable_console(void)
{
	/* Call VGA BIOS int10 function 0x4f14 to enable main console
	 * Epia-M does not always autosense the main console so forcing
	 * it on is good.
	 */

	/*                 int#,    EAX,    EBX,    ECX,    EDX,    ESI,    EDI */
	realmode_interrupt(0x10, 0x4f14, 0x8003, 0x0001, 0x0000, 0x0000, 0x0000);
}

extern u8 acpi_sleep_type;
static void vga_init(device_t dev)
{
	uint8_t reg8;

	mainboard_interrupt_handlers(0x15, &via_vx800_int15_handler);

	//A20 OPEN
	reg8 = inb(0x92);
	reg8 = reg8 | 2;
	outb(reg8, 0x92);

	//*
	//pci_write_config8(dev, 0x04, 0x07);
	//pci_write_config32(dev,0x10, 0xa0000008);
	//pci_write_config32(dev,0x14, 0xdd000000);
	pci_write_config32(dev, 0x10, VIACONFIG_VGA_PCI_10);
	pci_write_config32(dev, 0x14, VIACONFIG_VGA_PCI_14);
	pci_write_config8(dev, 0x3c, 0x0a);	//same with vx855_lpc.c
	//*/

	printk(BIOS_DEBUG, "Initializing VGA...\n");

	pci_dev_init(dev);

	printk(BIOS_DEBUG, "Enable VGA console\n");
	vga_enable_console();

	if ((acpi_sleep_type == 3)/* || (PAYLOAD_IS_SEABIOS == 0)*/) {
		/* It's not clear if these need to be programmed before or after
		 * the VGA bios runs. Try both, clean up later */
		/* Set memory rate to 200MHz */
		outb(0x3d, CRTM_INDEX);
		reg8 = inb(CRTM_DATA);
		reg8 &= 0x0f;
		reg8 |= (0x3 << 4);
		outb(0x3d, CRTM_INDEX);
		outb(reg8, CRTM_DATA);

#if 0
		/* Set framebuffer size to CONFIG_VIDEO_MB mb */
		reg8 = (CONFIG_VIDEO_MB/4);
		outb(0x39, SR_INDEX);
		outb(reg8, SR_DATA);
#endif
	}
}

static struct device_operations vga_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vga_init,
	.ops_pci = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops = &vga_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX855_VGA,
};
