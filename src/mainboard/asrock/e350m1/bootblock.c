/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5572d/nct5572d.h>

#define SERIAL_DEV PNP_DEV(0x2e, NCT5572D_SP1)

void bootblock_mainboard_early_init(void)
{
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
