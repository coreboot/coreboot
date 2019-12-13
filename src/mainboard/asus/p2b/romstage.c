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
 */

#include <console/console.h>
#include <southbridge/intel/i82371eb/i82371eb.h>
#include <northbridge/intel/i440bx/raminit.h>
#include <arch/romstage.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83977tf/w83977tf.h>
#include <cbmem.h>

#define SERIAL_DEV PNP_DEV(0x3f0, W83977TF_SP1)

int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

void mainboard_romstage_entry(void)
{
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	enable_smbus();
	sdram_initialize();
	cbmem_initialize_empty();
}
