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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <arch/interrupt.h>
#include "chip.h"
#include "northbridge.h"

static int via_vt8623_int15_handler(struct eregs *regs)
{
	int res=-1;
	printk(BIOS_DEBUG, "via_vt8623_int15_handler\n");
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

#ifdef UNUSED_CODE
static void write_protect_vgabios(void)
{
	device_t dev;

	printk(BIOS_DEBUG, "write_protect_vgabios\n");

	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3122, 0);
	if (dev)
		pci_write_config8(dev, 0x61, 0xaa);

	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3123, 0);
	if (dev)
		pci_write_config8(dev, 0x61, 0xaa);
}
#endif

static void vga_random_fixup(device_t dev)
{
	printk(BIOS_DEBUG, "VGA random fixup ...\n");
	pci_write_config8(dev, 0x04, 0x07);
	pci_write_config8(dev, 0x0d, 0x20);
	pci_write_config32(dev,0x10,0xd8000008);
	pci_write_config32(dev,0x14,0xdc000000);
}

static void vga_enable_console(void)
{
	/* Call VGA BIOS int10 function 0x4f14 to enable main console
	 * Epia-M does not always autosense the main console so forcing
	 * it on is good.
	 */

	/*                 int#,    EAX,    EBX,    ECX,    EDX,    ESI,    EDI */
	realmode_interrupt(0x10, 0x4f14, 0x8003, 0x0001, 0x0000, 0x0000, 0x0000);
}

static void vga_init(device_t dev)
{
	vga_random_fixup(dev);

	mainboard_interrupt_handlers(0x15, &via_vt8623_int15_handler);

#ifdef MEASURE_VGA_INIT_TIME
	msr_t clocks1, clocks2, instructions, setup;

	// set up performnce counters for debugging vga init sequence
	setup.lo = 0x1c0; // count instructions
	wrmsr(0x187,setup);
	instructions.hi = 0;
	instructions.lo = 0;
	wrmsr(0xc2,instructions);
	clocks1 = rdmsr(0x10);
#endif
	printk(BIOS_DEBUG, "Initializing VGA...\n");

	pci_dev_init(dev);

	printk(BIOS_DEBUG, "Enable VGA console\n");
	vga_enable_console();

#ifdef MEASURE_VGA_INIT_TIME
	clocks2 = rdmsr(0x10);
	instructions = rdmsr(0xc2);

	printk(BIOS_DEBUG, "Clocks 1 = %08x:%08x\n",clocks1.hi,clocks1.lo);
	printk(BIOS_DEBUG, "Clocks 2 = %08x:%08x\n",clocks2.hi,clocks2.lo);
	printk(BIOS_DEBUG, "Instructions = %08x:%08x\n",instructions.hi,instructions.lo);
#endif

	pci_write_config32(dev, 0x30, 0);

#if 0
	/* Set the vga mtrrs - disable for the moment as the add_var_mtrr function has vapourised */
	unsigned long fb;
	add_var_mtrr( 0xd0000000 >> 10, 0x08000000>>10, MTRR_TYPE_WRCOMB);
	fb = pci_read_config32(dev,0x10); // get the fb address
	add_var_mtrr( fb>>10, 8192, MTRR_TYPE_WRCOMB);
#endif
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
	.device = 0x3122,
};
