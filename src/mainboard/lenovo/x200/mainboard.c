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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <arch/io.h>
//#include <pc80/mc146818rtc.h>
#include <device/device.h>
#include <console/console.h>
#if CONFIG_VGA_ROM_RUN
#include <arch/interrupt.h>
#include <x86emu/x86emu.h>
#endif
#include <pc80/keyboard.h>
#include <ec/acpi/ec.h>
#include <smbios.h>
#include <string.h>
#include <build.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>

#include "hda_verb.h"

#if CONFIG_GENERATE_ACPI_TABLES
#include "cstates.c" /* Include it, as the linker won't find
			the overloaded weak function in there. */
#endif


#if CONFIG_VGA_ROM_RUN
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

	switch (X86_AX) {
	case 0x5f34:
		/* Set Panel Fitting Hook */
		X86_AX = 0x005f;
		X86_CX = PANEL_FIT_CENTERING;
		break;
	case 0x5f35:
		/* Boot Display Device Hook */
		X86_AX = 0x005f;
		X86_CX = BOOT_DISPLAY_DEFAULT; /* Select automatically. */
		break;
	case 0x5f40:
		/* Boot Panel Type Hook */
		/* Contrary to what EMGD's user's guide says,
		   this _alters_ the behavior of the Video BIOS. */
		/* LCD panel type is SIO GPIO40-43.
		   It's controlled by a DIP switch but was always
		   set to 4 while only values of 5 and 6 worked. */
		X86_AX = 0x005f;
		X86_CX = (inb(0x60f) & 0x0f) + 1;
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
			X86_AX, X86_BX, X86_CX, X86_DX);
		return 0;
	}

	/* Interrupt handled. */
	return 1;
}

#endif

static void verb_setup(void)
{
	cim_verb_data = mainboard_cim_verb_data;
	cim_verb_data_size = sizeof(mainboard_cim_verb_data);
	pc_beep_verbs = mainboard_pc_beep_verbs;
	pc_beep_verbs_size = ARRAY_SIZE(mainboard_pc_beep_verbs);
}

const char *smbios_mainboard_bios_version(void)
{
	/* Satisfy thinkpad_acpi.  */
	if (strlen(CONFIG_LOCALVERSION))
		return "CBET4000 " CONFIG_LOCALVERSION;
	else
		return "CBET4000 " COREBOOT_VERSION;
}

const char *smbios_mainboard_version(void)
{
	return "Lenovo X200";
}

static int mainboard_smbios_data(device_t dev, int *handle, unsigned long *current)
{
	int len;
	char tpec[] = "IBM ThinkPad Embedded Controller -[                 ]-";
	const char *oem_strings[] = {
		tpec,
	};

	h8_build_id_and_function_spec_version(tpec + 35, 17);
	len = smbios_write_type11(current, (*handle)++, oem_strings, ARRAY_SIZE(oem_strings));

	return len;
}

static void mainboard_init(device_t dev)
{
	/* This sneaked in here, because X200 SuperIO chip isn't really
	   connected to anything and hence we don't init it.
	 */
	pc_keyboard_init();
}

static void mainboard_enable(device_t dev)
{
	verb_setup();
#if CONFIG_VGA_ROM_RUN
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif

	dev->ops->get_smbios_data = mainboard_smbios_data;
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

