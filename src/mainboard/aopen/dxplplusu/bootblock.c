/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/smsc/lpc47m10x/lpc47m10x.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M10X2_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Get the serial port configured. */
	lpc47m10x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
