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

#include <arch/io.h>
#include <southbridge/intel/i82371eb/i82371eb.h>
#include <northbridge/intel/i440bx/raminit.h>
#include <superio/winbond/common/winbond.h>
/* FIXME: The ASUS P3B-F has a Winbond W83977EF, actually. */
#include <superio/winbond/w83977tf/w83977tf.h>

/* FIXME: The ASUS P3B-F has a Winbond W83977EF, actually. */
#define SERIAL_DEV PNP_DEV(0x3f0, W83977TF_SP1)

/*
 * ASUS P3B-F specific SPD enable magic.
 *
 * Setting the byte at offset 0x37 in the PM I/O space to 0x6f will make the
 * board DIMMs accessible at SMBus/SPD offsets 0x50-0x53. Per default the SPD
 * offsets 0x50-0x53 are _not_ readable (all SPD reads will return 0xff) which
 * will make RAM init fail.
 *
 * Tested values for PM I/O offset 0x37:
 * 0x67: 11 00 111: Only SMBus/I2C offsets 0x48/0x49/0x2d accessible
 * 0x6f: 11 01 111: Only SMBus/I2C offsets 0x50-0x53 (SPD) accessible
 * 0x77: 11 10 111: Only SMBus/I2C offset 0x69 accessible
 *
 * PM I/O space offset 0x37 is GPOREG[31:24], i.e. it controls the GPIOs
 * 24-30 of the PIIX4E (bit 31 is reserved). Thus, GPIOs 27 and 28
 * control which SMBus/I2C offsets can be accessed.
 */
void enable_spd(void)
{
	outb(0x6f, PM_IO_BASE + 0x37);
}

/*
 * Disable SPD access after RAM init to allow access to SMBus/I2C offsets
 * 0x48/0x49/0x2d, which is required e.g. by lm-sensors.
 */
void disable_spd(void)
{
	outb(0x67, PM_IO_BASE + 0x37);
}

void mainboard_enable_serial(void)
{
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
