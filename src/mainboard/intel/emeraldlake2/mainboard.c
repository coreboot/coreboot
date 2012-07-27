/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#if defined(CONFIG_PCI_OPTION_ROM_RUN_YABEL) && CONFIG_PCI_OPTION_ROM_RUN_YABEL
#include <x86emu/x86emu.h>
#endif
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <arch/coreboot_tables.h>
#include "hda_verb.h"
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	outb(0xcb, 0xb2);
}

int add_mainboard_resources(struct lb_memory *mem)
{
	return 0;
}

#if defined(CONFIG_PCI_OPTION_ROM_RUN_REALMODE) && CONFIG_PCI_OPTION_ROM_RUN_REALMODE
static int int15_handler(struct eregs *regs)
{
	int res=-1;

	printk(BIOS_DEBUG, "%s: INT15 function %04x!\n",
			__func__, regs->eax & 0xffff);

	switch(regs->eax & 0xffff) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video bios default
		 */
		regs->eax &= 0xffff0000;
		regs->eax |= 0x005f;
		regs->ecx &= 0xffffff00;
		regs->ecx |= 0x01;
		res = 0;
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
		regs->eax &= 0xffff0000;
		regs->eax |= 0x005f;
		regs->ecx &= 0xffff0000;
		regs->ecx |= 0x0000;
		res = 0;
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SVDO-LVDS, LFP driven by SVDO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		regs->eax &= 0xffff0000;
		regs->eax |= 0x005f;
		regs->ecx &= 0xffff0000;
		regs->ecx |= 0x0003;
		res = 0;
		break;
	case 0x5f70:
		switch ((regs->ecx >> 8) & 0xff) {
		case 0:
			/* Get Mux */
			regs->eax &= 0xffff0000;
			regs->eax |= 0x005f;
			regs->ecx &= 0xffff0000;
			regs->ecx |= 0x0000;
			res = 0;
			break;
		case 1:
			/* Set Mux */
			regs->eax &= 0xffff0000;
			regs->eax |= 0x005f;
			regs->ecx &= 0xffff0000;
			regs->ecx |= 0x0000;
			res = 0;
			break;
		case 2:
			/* Get SG/Non-SG mode */
			regs->eax &= 0xffff0000;
			regs->eax |= 0x005f;
			regs->ecx &= 0xffff0000;
			regs->ecx |= 0x0000;
			res = 0;
			break;
		default:
			/* Interrupt was not handled */
			printk(BIOS_DEBUG, "Unknown INT15 5f70 function: 0x%02x\n",
				((regs->ecx >> 8) & 0xff));
			return 0;
		}
		break;

        default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
				regs->eax & 0xffff);
		break;
	}
	return res;
}
#endif

#if defined(CONFIG_PCI_OPTION_ROM_RUN_YABEL) && CONFIG_PCI_OPTION_ROM_RUN_YABEL
static int int15_handler(void)
{
	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
			  __func__, M.x86.R_AX, M.x86.R_BX, M.x86.R_CX, M.x86.R_DX);

	switch (M.x86.R_AX) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 */
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = 0x0001;
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
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = 0x0000;
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SVDO-LVDS, LFP driven by SVDO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = 3;
		break;
	case 0x5f70:
		/* Unknown */
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = 0;
		break;
	default:
		/* Interrupt was not handled */
		printk(BIOS_DEBUG, "Unknown INT15 function: 0x%04x\n",
			M.x86.R_AX);
		return 0;
	}

	/* Interrupt handled */
	return 1;
}
#endif

#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
static void int15_install(void)
{
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
	typedef int (* yabel_handleIntFunc)(void);
	extern yabel_handleIntFunc yabel_intFuncArray[256];
	yabel_intFuncArray[0x15] = int15_handler;
#endif
#ifdef CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
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

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(device_t dev)
{
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	/* Install custom int15 handler for VGA OPROM */
	int15_install();
#endif
	verb_setup();
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("Compal Link ChromeBox")
	.enable_dev = mainboard_enable,
};

