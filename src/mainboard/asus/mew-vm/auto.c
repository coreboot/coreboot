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

#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "superio/smsc/lpc47b272/lpc47b272_early_serial.c"
#include "northbridge/intel/i82810/raminit.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, LPC47B272_SP1)

#include "southbridge/intel/i82801xx/i82801xx_early_smbus.c"

/* TODO: Not needed? */
void udelay(int usecs) 
{
	int i;
	for (i = 0; i < usecs; i++)
		outb(i&0xff, 0x80);
}

#include "debug.c"
#include "lib/delay.c"

#include "northbridge/intel/i82810/raminit.c"
#include "northbridge/intel/i82810/debug.c"
#include "sdram/generic_sdram.c"

static void main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
		 .d0 = PCI_DEV(0, 0, 0),
		 .channel0 = {0x50, 0x51},
		 }
	};

	if (bist == 0)
		early_mtrr_init();

	enable_smbus();

	lpc47b272_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	/* Halt if there was a built in self test failure. */
	report_bist_failure(bist);

	/* dump_spd_registers(&memctrl[0]); */

	/* sdram_initialize() runs out of registers. */
	/* sdram_initialize(ARRAY_SIZE(memctrl), memctrl); */

	sdram_set_registers(memctrl);
	sdram_set_spd_registers(memctrl);
	sdram_enable(0, memctrl);

	/* Check RAM. */
	/* ram_check(0, 640 * 1024); */
}
