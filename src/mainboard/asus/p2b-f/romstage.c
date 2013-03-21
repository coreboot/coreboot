/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
#include <stdlib.h>
#include <console/console.h>
#include "southbridge/intel/i82371eb/i82371eb.h"
#include "northbridge/intel/i440bx/raminit.h"
#include "drivers/pc80/udelay_io.c"
#include "lib/delay.c"
#include "cpu/x86/bist.h"
/* FIXME: The ASUS P2B-F has a Winbond W83977EF, actually. */
#include "superio/winbond/w83977tf/early_serial.c"
#include <lib.h>

/* FIXME: The ASUS P2B-F has a Winbond W83977EF, actually. */
#define SERIAL_DEV PNP_DEV(0x3f0, W83977TF_SP1)

int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

void main(unsigned long bist)
{
	/* FIXME: The ASUS P2B-F has a Winbond W83977EF, actually. */
	w83977tf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	report_bist_failure(bist);

	enable_smbus();
	dump_spd_registers();
	sdram_set_registers();
	sdram_set_spd_registers();
	sdram_enable();
}
