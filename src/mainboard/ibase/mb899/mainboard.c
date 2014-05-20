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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <device/device.h>
#include <console/console.h>
#if CONFIG_VGA_ROM_RUN
#include <x86emu/x86emu.h>
#endif
#include <pc80/mc146818rtc.h>
#include <arch/io.h>
#include <arch/interrupt.h>

#include "superio_hwm.h"

#if CONFIG_VGA_ROM_RUN
static int int15_handler(void)
{
#define BOOT_DISPLAY_DEFAULT	0
#define BOOT_DISPLAY_CRT	(1 << 0)
#define BOOT_DISPLAY_TV		(1 << 1)
#define BOOT_DISPLAY_EFP	(1 << 2)
#define BOOT_DISPLAY_LCD	(1 << 3)
#define BOOT_DISPLAY_CRT2	(1 << 4)
#define BOOT_DISPLAY_TV2	(1 << 5)
#define BOOT_DISPLAY_EFP2	(1 << 6)
#define BOOT_DISPLAY_LCD2	(1 << 7)

	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
			  __func__, X86_AX, X86_BX, X86_CX, X86_DX);

	switch (X86_AX) {
	case 0x5f35: /* Boot Display */
		X86_AX = 0x005f; // Success
		X86_CL = BOOT_DISPLAY_DEFAULT;
		break;
	case 0x5f40: /* Boot Panel Type */
		// M.x86.R_AX = 0x015f; // Supported but failed
		X86_AX = 0x005f; // Success
		X86_CL = 3; // Display ID
		break;
	default:
		/* Interrupt was not handled */
		return 0;
	}

	/* Interrupt handled */
	return 1;
}
#endif

/* Audio Setup */

extern u32 * cim_verb_data;
extern u32 cim_verb_data_size;

static void verb_setup(void)
{
	// Default VERB is fine on this mainboard.
	cim_verb_data = NULL;
	cim_verb_data_size = 0;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(device_t dev)
{
#if CONFIG_VGA_ROM_RUN
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
	verb_setup();
	hwm_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
