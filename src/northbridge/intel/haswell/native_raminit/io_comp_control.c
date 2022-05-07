/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <northbridge/intel/haswell/haswell.h>
#include <timer.h>
#include <types.h>

#include "raminit_native.h"

enum raminit_status io_reset(void)
{
	union mc_init_state_g_reg mc_init_state_g = {
		.raw = mchbar_read32(MC_INIT_STATE_G),
	};
	mc_init_state_g.reset_io = 1;
	mchbar_write32(MC_INIT_STATE_G, mc_init_state_g.raw);
	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, 2000);
	do {
		mc_init_state_g.raw = mchbar_read32(MC_INIT_STATE_G);
		if (mc_init_state_g.reset_io == 0)
			return RAMINIT_STATUS_SUCCESS;

	} while (!stopwatch_expired(&timer));
	printk(BIOS_ERR, "Timed out waiting for DDR I/O reset to complete\n");
	return RAMINIT_STATUS_POLL_TIMEOUT;
}

enum raminit_status wait_for_first_rcomp(void)
{
	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, 2000);
	do {
		if (mchbar_read32(RCOMP_TIMER) & BIT(16))
			return RAMINIT_STATUS_SUCCESS;

	} while (!stopwatch_expired(&timer));
	printk(BIOS_ERR, "Timed out waiting for RCOMP to complete\n");
	return RAMINIT_STATUS_POLL_TIMEOUT;
}
