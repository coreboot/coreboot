/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

#include <stdint.h>
#include <stdlib.h>
#include <arch/io.h>
//#include <pc80/mc146818rtc.h>
#include <device/device.h>
#include <console/console.h>
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
#include <x86emu/x86emu.h>
#endif
#include <pc80/keyboard.h>
#include <ec/acpi/ec.h>
#include "hda_verb.h"

#if CONFIG_GENERATE_ACPI_TABLES
#include "cstates.c" /* Include it, as the linker won't find
			the overloaded weak function in there. */
#endif


#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
static int int15_handler(void)
{
	enum {
		BOOT_DISPLAY_DEFAULT	 = 0,
		BOOT_DISPLAY_CRT	 = (1 << 0),
		BOOT_DISPLAY_TV		 = (1 << 1),
		BOOT_DISPLAY_EFP	 = (1 << 2),
		BOOT_DISPLAY_LFP	 = (1 << 3),
		BOOT_DISPLAY_CRT2	 = (1 << 4),
		BOOT_DISPLAY_TV2	 = (1 << 5),
		BOOT_DISPLAY_EFP2	 = (1 << 6),
		BOOT_DISPLAY_LFP2	 = (1 << 7),
	};
	enum {
		PANEL_FIT_DEFAULT	= 0,
		PANEL_FIT_CENTERING	= (1 << 0),
		PANEL_FIT_TXT_STRETCH	= (1 << 1),
		PANEL_FIT_GFX_STRETCH	= (1 << 2),
	};

	switch (M.x86.R_AX) {
	case 0x5f34:
		/* Set Panel Fitting Hook */
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = PANEL_FIT_CENTERING;
		break;
	case 0x5f35:
		/* Boot Display Device Hook */
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = BOOT_DISPLAY_DEFAULT; /* Select automatically. */
		break;
	case 0x5f40:
		/* Boot Panel Type Hook */
		/* Contrary to what EMGD's user's guide says,
		   this _alters_ the behavior of the Video BIOS. */
		/* LCD panel type is SIO GPIO40-43.
		   It's controlled by a DIP switch but was always
		   set to 4 while only values of 5 and 6 worked. */
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = (inb(0x60f) & 0x0f) + 1;
		break;
	case 0x5f70:
		/* Sandybridge boards return 0 here. */
	case 0x5f14:
	case 0x5f21:
	case 0x5f22:
	case 0x5f49:
		/* No documentation found. */
	default:
		/* Interrupt was not handled. */
		printk(BIOS_DEBUG,
			"%s: AX=%04x BX=%04x CX=%04x DX=%04x\n", __func__,
			M.x86.R_AX, M.x86.R_BX, M.x86.R_CX, M.x86.R_DX);
		return 0;
	}

	/* Interrupt handled. */
	return 1;
}

static void int15_install(void)
{
	typedef int (* yabel_handleIntFunc)(void);
	extern yabel_handleIntFunc yabel_intFuncArray[256];
	yabel_intFuncArray[0x15] = int15_handler;
}
#endif

static void verb_setup(void)
{
	cim_verb_data = mainboard_cim_verb_data;
	cim_verb_data_size = sizeof(mainboard_cim_verb_data);
	pc_beep_verbs = mainboard_pc_beep_verbs;
	pc_beep_verbs_size = ARRAY_SIZE(mainboard_pc_beep_verbs);
}

static void ec_setup(void)
{
	/* Thermal limits?  Values are from ectool's ram dump. */
	ec_write(0xd1, 0x57); /* CPUH */
	ec_write(0xd2, 0xc9); /* CPUL */
	ec_write(0xd4, 0x64); /* SYSH */
	ec_write(0xd5, 0xc9); /* SYSL */

	send_ec_command(0x04); /* Set_SMI_Enable */
	send_ec_command(0xab); /* Set_ACPI_Disable */
	send_ec_command(0xac); /* Clr_SYS_Flag? well, why not? */
	send_ec_command(0xad); /* Set_Thml_Value */
}

static void mainboard_enable(device_t dev)
{
	ec_setup();
	verb_setup();
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL// || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	/* Install custom int15 handler for VGA OPROM */
	int15_install();
#endif

	/* We have no driver for the embedded controller since the firmware
	   does most of the job. Hence, initialize keyboards here. */
	pc_keyboard_init(NULL);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

