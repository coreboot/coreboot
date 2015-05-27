/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011-2012 Google Inc.
 * Copyright (C) 2014 Vladimir Serbinenko
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

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <drivers/intel/gma/int15.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <device/pci.h>
#include <cbfs.h>
#include <pc80/keyboard.h>
#include <ec/lenovo/h8/h8.h>

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	outb(0xcb, 0xb2);
}

static void mainboard_init(device_t dev)
{
	RCBA32(0x38c8) = 0x00002005;
	RCBA32(0x38c4) = 0x00802005;
	RCBA32(0x38c0) = 0x00000007;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(device_t dev)
{
	dev->ops->init = mainboard_init;

	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

void h8_mainboard_init_dock (void)
{
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
