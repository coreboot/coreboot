/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <types.h>
#include <console.h>
#include <arch/x86/mtrr.h>
#include <arch/x86/legacy.h>
#include <device/pci.h>
#include <superio/fintek/f71805f/f71805f.h>
#include <northbridge/via/cn700/cn700.h>
#include <southbridge/via/vt8237/vt8237.h>

#define SMBUS_IO_BASE	0x0400

void hardware_stage1(void)
{
	post_code(POST_START_OF_MAIN);
	f71805f_enable_serial(0x2e);
	uart_init();

	printk(BIOS_SPEW, "In hardware_stage1()\n");
	cn700_stage1();
	vt8237_stage1(SMBUS_IO_BASE);
}

void mainboard_pre_payload(void)
{
}
