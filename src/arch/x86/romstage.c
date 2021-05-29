/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <timestamp.h>
#include <romstage_common.h>

asmlinkage void car_stage_entry(void)
{
	timestamp_add_now(TS_ROMSTAGE_START);

	/* Assumes the hardware was set up during the bootblock */
	console_init();

	romstage_main();
}
