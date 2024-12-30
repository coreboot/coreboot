/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/nuvoton/npcd378/npcd378.h>
#include <superio/nuvoton/common/nuvoton.h>

#define SERIAL_DEV PNP_DEV(0x2e, NPCD378_SP2)

void bootblock_mainboard_early_init(void)
{
	if (CONFIG(CONSOLE_SERIAL))
		nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
