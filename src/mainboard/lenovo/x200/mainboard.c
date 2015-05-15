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
#include <device/device.h>
#include <console/console.h>
#include <drivers/intel/gma/int15.h>
#include <pc80/keyboard.h>
#include <ec/acpi/ec.h>
#include <smbios.h>
#include <string.h>
#include <version.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include "drivers/lenovo/lenovo.h"

#include "cstates.c" /* Include it, as the linker won't find
			the overloaded weak function in there. */

const char *smbios_mainboard_bios_version(void)
{
	static char *s = NULL;

	/* Satisfy thinkpad_acpi.  */
	if (strlen(CONFIG_LOCALVERSION))
		return "CBET4000 " CONFIG_LOCALVERSION;

	if (s != NULL)
		return s;
	s = strconcat("CBET4000 ", coreboot_version);
	return s;
}

static void fill_ssdt(void)
{
	drivers_lenovo_serial_ports_ssdt_generate("\\_SB.PCI0.LPCB", 0);
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
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_CENTERING, GMA_INT15_BOOT_DISPLAY_DEFAULT, 2);

	dev->ops->init = mainboard_init;
	dev->ops->acpi_fill_ssdt_generator = fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

