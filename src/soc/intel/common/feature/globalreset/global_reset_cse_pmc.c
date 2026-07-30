/* SPDX-License-Identifier: GPL-2.0-only */

#include <cf9_reset.h>
#include <console/console.h>
#include <delay.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <soc/intel/common/reset.h>

/* Time to give CSE to actually perform the reset once it acked the request. */
#define CSE_GLOBAL_RESET_TIMEOUT_MS 1000

void do_global_reset(void)
{
	/* Ask CSE to do the global reset */
	if (cse_request_global_reset() == CSE_TX_RX_SUCCESS) {
		/*
		 * CSE acking the request only means it accepted the message; it does
		 * not guarantee the platform reset it triggers actually happens (see
		 * coreboot Bug #655). Give CSE a chance to reset the system and fall
		 * back to the PMC/CF9 reset below if it doesn't come through.
		 */
		mdelay(CSE_GLOBAL_RESET_TIMEOUT_MS);
	}

	/* global reset if CSE fail to reset */
	pmc_global_reset_enable(1);
	do_full_reset();
}
