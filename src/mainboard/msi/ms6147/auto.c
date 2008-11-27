/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Mats Erik Andersson <mats.andersson@gisladisker.org>
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
#include <stdlib.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "southbridge/intel/i82371eb/i82371eb_early_smbus.c"
#include "northbridge/intel/i440bx/raminit.h"
#include "lib/debug.c"
#include "pc80/udelay_io.c"
#include "lib/delay.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include <cpu/x86/bist.h>
#include "superio/winbond/w83977tf/w83977tf_early_serial.c"

#define SERIAL_DEV PNP_DEV(0x3f0, W83977TF_SP1)

static inline int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/i440bx/raminit.c"
/* #include "northbridge/intel/i440bx/debug.c" */

static void main(unsigned long bist)
{
	if (bist == 0)
		early_mtrr_init();

	w83977tf_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();
	report_bist_failure(bist);
	enable_smbus();
	/* dump_spd_registers(); */
	sdram_set_registers();
	sdram_set_spd_registers();
	sdram_enable();
#if 0
	ram_check(0, 640 * 1024);          /* DOS-area */
	ram_check(0x00100000, 0x00400000); /* 1MB to 4MB */
	ram_check(0x00100000, 0x03ffffff); /* 1MB to 64MB- */
	ram_check(0x03fff000, 0x04000010); /* Across 64MB boundary */
	ram_check(0x07ffff00, 0x07fffff0); /* Just below 128MB */
	ram_check(0x00100000, 0x07ffffff); /* 1MB to 128MB- */
#endif
}
