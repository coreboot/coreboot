/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Nikolay Petukhov <nikolay.petukhov@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include "superio/winbond/w83977f/early_serial.c"
#include "southbridge/amd/cs5530/enable_rom.c"
#include "cpu/x86/bist.h"
#include "drivers/pc80/udelay_io.c"
#include "northbridge/amd/gx1/raminit.c"

#define SERIAL_DEV PNP_DEV(0x3f0, W83977F_SP1)

static void main(unsigned long bist)
{
	w83977f_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	report_bist_failure(bist);

	/* Disable Watchdog Timer. */
	inb(0x043);
	inb(0x843);

	cs5530_enable_rom();
	sdram_init();
}
