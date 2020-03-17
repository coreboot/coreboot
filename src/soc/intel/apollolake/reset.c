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

#include <cf9_reset.h>
#include <console/console.h>
#include <delay.h>
#include <fsp/util.h>
#include <intelblocks/pmclib.h>
#include <soc/heci.h>
#include <soc/intel/common/reset.h>
#include <soc/pm.h>
#include <timer.h>

#define CSE_WAIT_MAX_MS							1000

void do_global_reset(void)
{
	pmc_global_reset_enable(1);
	do_full_reset();
}

void cf9_reset_prepare(void)
{
	struct stopwatch sw;

	/*
	 * If CSE state is something else than 'normal', it is probably in some
	 * recovery state. In this case there is no point in  waiting for it to
	 * get ready so we cross fingers and reset.
	 */
	if (!heci_cse_normal()) {
		printk(BIOS_DEBUG, "CSE is not in normal state, resetting\n");
		return;
	}

	/* Reset if CSE is ready */
	if (heci_cse_done())
		return;

	printk(BIOS_SPEW, "CSE is not yet ready, waiting\n");
	stopwatch_init_msecs_expire(&sw, CSE_WAIT_MAX_MS);
	while (!heci_cse_done()) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_SPEW, "CSE timed out. Resetting\n");
			return;
		}
		mdelay(1);
	}
	printk(BIOS_SPEW, "CSE took %lu ms\n", stopwatch_duration_msecs(&sw));
}

void chipset_handle_reset(uint32_t status)
{
	switch (status) {
	case FSP_STATUS_RESET_REQUIRED_5: /* Global Reset */
		global_reset();
		break;
	default:
		printk(BIOS_ERR, "unhandled reset type %x\n", status);
		die("unknown reset type");
		break;
	}
}
