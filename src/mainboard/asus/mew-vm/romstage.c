/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey@slightlyhackish.com>
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

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include "pc80/serial.c"
#include "console/console.c"
#include "lib/ramtest.c"
#include "superio/smsc/lpc47b272/lpc47b272_early_serial.c"
#include "northbridge/intel/i82810/raminit.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, LPC47B272_SP1)

#include "southbridge/intel/i82801ax/i82801ax_early_smbus.c"
#include "lib/debug.c"
#include "pc80/udelay_io.c"
#include "lib/delay.c"
#include "northbridge/intel/i82810/raminit.c"
#include "northbridge/intel/i82810/debug.c"

static void main(unsigned long bist)
{
	if (bist == 0)
		early_mtrr_init();

	lpc47b272_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	uart_init();
	console_init();

	enable_smbus();

	/* Halt if there was a built in self test failure. */
	report_bist_failure(bist);

	/* dump_spd_registers(); */

	sdram_set_registers();
	sdram_set_spd_registers();
	sdram_enable();

	/* Check RAM. */
	/* ram_check(0, 640 * 1024); */
}

