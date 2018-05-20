/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <arch/interrupt.h>
#include "registers.h"
#include <x86emu/regs.h>
#if IS_ENABLED(CONFIG_PCI_OPTION_ROM_RUN_REALMODE)
#include <device/oprom/realmode/x86.h>
#endif

/* PCI Domain 1 Device 0 Function 0 */

#define SR_INDEX	0x3c4
#define SR_DATA		0x3c5
#define CRTM_INDEX	0x3b4
#define CRTM_DATA	0x3b5
#define CRTC_INDEX	0x3d4
#define CRTC_DATA	0x3d5

static int via_cx700_int15_handler(void)
{
	int res = 0;
	u8 mem_speed;

#define MEMORY_SPEED_66MHZ	(0 << 4)
#define MEMORY_SPEED_100MHZ	(1 << 4)
#define MEMORY_SPEED_133MHZ	(1 << 4)
#define MEMORY_SPEED_200MHZ	(3 << 4) // DDR200
#define MEMORY_SPEED_266MHZ	(4 << 4) // DDR266
#define MEMORY_SPEED_333MHZ	(5 << 4) // DDR333
#define MEMORY_SPEED_400MHZ	(6 << 4) // DDR400
#define MEMORY_SPEED_533MHZ	(7 << 4) // DDR533
#define MEMORY_SPEED_667MHZ	(8 << 4) // DDR667

	const u8 memory_mapping[6] = {
		MEMORY_SPEED_200MHZ, MEMORY_SPEED_266MHZ,
		MEMORY_SPEED_333MHZ, MEMORY_SPEED_400MHZ,
		MEMORY_SPEED_533MHZ, MEMORY_SPEED_667MHZ
	};

	printk(BIOS_DEBUG, "via_cx700_int15_handler\n");

	switch(X86_EAX & 0xffff) {
	case 0x5f00:	/* VGA POST Initialization Signal */
		X86_EAX = (X86_EAX & 0xffff0000 ) | 0x5f;
		res = 1;
		break;

	case 0x5f01:	/* Software Panel Type Configuration */
		X86_EAX = (X86_EAX & 0xffff0000 ) | 0x5f;
		// panel type =  2 = 1024 * 768
		X86_ECX = (X86_ECX & 0xffffff00 ) | 2;
		res = 1;
		break;

	case 0x5f27:	/* Boot Device Selection */
		X86_EAX = (X86_EAX & 0xffff0000 ) | 0x5f;

		X86_EBX = 0x00000000; // 0 -> default
		X86_ECX = 0x00000000; // 0 -> default
		// TV Layout - default
		X86_EDX = (X86_EDX & 0xffffff00) | 0;
		res = 1;
		break;

	case 0x5f0b:	/* Get Expansion Setting */
		X86_EAX = (X86_EAX & 0xffff0000 ) | 0x5f;

		X86_ECX = X86_ECX & 0xffffff00; // non-expansion
		// regs->ecx = regs->ecx & 0xffffff00 | 1; // expansion
		res = 1;
		break;

	case 0x5f0f:	/* VGA Post Completion */
		X86_EAX = (X86_EAX & 0xffff0000 ) | 0x5f;
		res = 1;
		break;

	case 0x5f18:
		X86_EAX = (X86_EAX & 0xffff0000 ) | 0x5f;
#define UMA_SIZE_8MB		(3 << 0)
#define UMA_SIZE_16MB		(4 << 0)
#define UMA_SIZE_32MB		(5 << 0)

		X86_EBX = (X86_EBX & 0xffff0000 ) | MEMORY_SPEED_533MHZ | UMA_SIZE_32MB;

		mem_speed = pci_read_config8(dev_find_slot(0, PCI_DEVFN(0, 4)), SCRATCH_DRAM_FREQ);
		if (mem_speed > 5)
			mem_speed = 5;

		X86_EBX |= memory_mapping[mem_speed];

		res = 1;
		break;

	default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
				X86_EAX & 0xffff);
		break;
	}
	return res;
}

#ifdef UNUSED_CODE
static void write_protect_vgabios(void)
{
	struct device *dev;

	printk(BIOS_DEBUG, "write_protect_vgabios\n");

	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3324, 0);
	if (dev)
		pci_write_config8(dev, 0x80, 0xff);

	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x7324, 0);
	if (dev)
		pci_write_config8(dev, 0x61, 0xff);
}
#endif

static void vga_enable_console(void)
{
#if IS_ENABLED(CONFIG_PCI_OPTION_ROM_RUN_REALMODE)
	/* Call VGA BIOS int10 function 0x4f14 to enable main console
	 * Epia-M does not always autosense the main console so forcing
	 * it on is good.
	 */

	/*                 int#,    EAX,    EBX,    ECX,    EDX,    ESI,    EDI */
	realmode_interrupt(0x10, 0x4f14, 0x8003, 0x0001, 0x0000, 0x0000, 0x0000);
#endif
}

static void vga_init(struct device *dev)
{
	u8 reg8;

	mainboard_interrupt_handlers(0x15, &via_cx700_int15_handler);

	//*
	pci_write_config8(dev, 0x04, 0x07);
	pci_write_config8(dev, 0x3e, 0x02);
	pci_write_config8(dev, 0x0d, 0x40);
	pci_write_config32(dev, 0x10, 0xa0000008);
	pci_write_config32(dev, 0x14, 0xdd000000);
	pci_write_config8(dev, 0x3c, 0x0b);
	//*/

	printk(BIOS_DEBUG, "Initializing VGA...\n");

	pci_dev_init(dev);

	if (pci_read_config32(dev, PCI_ROM_ADDRESS) != 0xc0000) return;

	printk(BIOS_DEBUG, "Enable VGA console\n");
	vga_enable_console();

	/* It's not clear if these need to be programmed before or after
	 * the VGA bios runs. Try both, clean up later */
	/* Set memory rate to 200MHz */
	outb(0x3d, CRTM_INDEX);
	reg8 = inb(CRTM_DATA);
	reg8 &= 0x0f;
	reg8 |= (0x3 << 4);
	outb(0x3d, CRTM_INDEX);
	outb(reg8, CRTM_DATA);

	/* Set framebuffer size to 32mb */
	reg8 = (32 / 4);
	outb(0x39, SR_INDEX);
	outb(reg8, SR_DATA);
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
	.device = 0x3157,
};
