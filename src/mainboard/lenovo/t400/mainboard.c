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
 * Foundation, Inc.
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
#include <ec/lenovo/pmh7/pmh7.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include "drivers/lenovo/lenovo.h"

static void fill_ssdt(void)
{
	drivers_lenovo_serial_ports_ssdt_generate("\\_SB.PCI0.LPCB", 0);
}

static void mainboard_enable(device_t dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_CENTERING, GMA_INT15_BOOT_DISPLAY_DEFAULT, 2);

	dev->ops->acpi_fill_ssdt_generator = fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

