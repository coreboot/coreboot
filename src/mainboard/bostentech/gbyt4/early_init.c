/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SERIAL_DEV PNP_DEV(0x2e, 0x01)

void bootblock_mainboard_early_init(void)
{
	/* Enable serial port */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
