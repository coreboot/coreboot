/*
 * This file is part of the coreboot project.
 *
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

#include <northbridge/intel/i440bx/raminit.h>
#include <superio/winbond/common/winbond.h>
/* FIXME: The ASUS P2B-LS has a Winbond W83977EF, actually. */
#include <superio/winbond/w83977tf/w83977tf.h>

#define SERIAL_DEV PNP_DEV(0x3f0, W83977TF_SP1)

void mainboard_enable_serial(void)
{
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
