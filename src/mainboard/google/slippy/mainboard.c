/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#if CONFIG_PCI_ROM_RUN || CONFIG_VGA_ROM_RUN
#include <x86emu/x86emu.h>
#endif
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include "hda_verb.h"
#include <southbridge/intel/lynxpoint/pch.h>
#include "ec.h"

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	outb(0xcb, 0xb2);
}

#if CONFIG_PCI_ROM_RUN || CONFIG_VGA_ROM_RUN
static int int15_handler(void)
{
	int res = 0;

	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
	       __func__, X86_AX, X86_BX, X86_CX, X86_DX);

	switch (X86_AX) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video bios default
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0001;
		res = 1;
		break;
	case 0x5f35:
		/*
		 * Boot Display Device Hook:
		 *  bit 0 = CRT
		 *  bit 1 = TV (eDP) *
		 *  bit 2 = EFP *
		 *  bit 3 = LFP
		 *  bit 4 = CRT2
		 *  bit 5 = TV2 (eDP) *
		 *  bit 6 = EFP2 *
		 *  bit 7 = LFP2
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0000;
		res = 1;
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SVDO-LVDS, LFP driven by SVDO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0003;
		res = 1;
		break;
	case 0x5f70:
		switch ((X86_CX >> 8) & 0xff) {
		case 0:
			/* Get Mux */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		case 1:
			/* Set Mux */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		case 2:
			/* Get SG/Non-SG mode */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		default:
			/* Interrupt was not handled */
			printk(BIOS_DEBUG,
			       "Unknown INT15 5f70 function: 0x%02x\n",
				((X86_CX >> 8) & 0xff));
			break;
		}
		break;

        default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n", X86_AX);
		break;
	}
	return res;
}
#endif

/* Audio Setup */

extern const u32 * cim_verb_data;
extern u32 cim_verb_data_size;

static void verb_setup(void)
{
	cim_verb_data = mainboard_cim_verb_data;
	cim_verb_data_size = sizeof(mainboard_cim_verb_data);
}

static void mainboard_init(device_t dev)
{
	mainboard_ec_init();
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(device_t dev)
{
	dev->ops->init = mainboard_init;
#if CONFIG_PCI_ROM_RUN || CONFIG_VGA_ROM_RUN
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
	verb_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

