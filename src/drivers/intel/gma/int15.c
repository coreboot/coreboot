/* SPDX-License-Identifier: GPL-2.0-only */

#include <x86emu/x86emu.h>
#include <arch/interrupt.h>
#include <console/console.h>

#include "int15.h"

static int active_lfp, pfit, display, panel_type;

int intel_vga_int15_handler(void)
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
		 *  0     = video BIOS default
		 */
		X86_AX = 0x005f;
		X86_CX = pfit;
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
		X86_CX = display;
		res = 1;
		break;
	case 0x5f40: /* Boot Panel Type */
		X86_AX = 0x005f; // Success
		X86_CL = panel_type;
		printk(BIOS_DEBUG, "DISPLAY=%x\n", X86_CL);
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
		X86_CX = active_lfp;
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

void install_intel_vga_int15_handler(int active_lfp_, int pfit_, int display_, int panel_type_)
{
	active_lfp = active_lfp_;
	pfit = pfit_;
	display = display_;
	panel_type = panel_type_;
	mainboard_interrupt_handlers(0x15, &intel_vga_int15_handler);
}
