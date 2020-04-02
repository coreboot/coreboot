/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>

void bootblock_mainboard_early_init(void)
{
#if 0
	volatile u32 i, val;

	/* LPC clock? Should happen before enable_serial. */

	/*
	* On Larne, after LpcClkDrvSth is set, it needs some time to be stable,
	* because of the buffer ICS551M
	*/
	for (i = 0; i < 200000; i++)
		val = inb(0xcd6);
#endif
}
