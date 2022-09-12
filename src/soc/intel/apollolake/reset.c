/* SPDX-License-Identifier: GPL-2.0-only */

#include <cf9_reset.h>
#include <console/console.h>
#include <delay.h>
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
	printk(BIOS_SPEW, "CSE took %lld ms\n", stopwatch_duration_msecs(&sw));
}
