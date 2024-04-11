/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <northbridge/intel/haswell/haswell.h>
#include <timer.h>
#include <types.h>

#include "raminit_native.h"

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
