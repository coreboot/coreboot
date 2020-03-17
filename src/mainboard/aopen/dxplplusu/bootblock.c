/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <superio/smsc/lpc47m10x/lpc47m10x.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M10X2_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Get the serial port configured. */
	lpc47m10x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
