/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <console/console.h>
#include "bmcinfo.h"

/*
 * Display board serial early
 */

void bootblock_mainboard_init(void)
{
	if (CONFIG(BOOTBLOCK_CONSOLE))
		printk(BIOS_SPEW, "Board Serial: %s.\n", bmcinfo_serial());
}
