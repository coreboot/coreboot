/* SPDX-License-Identifier: GPL-2.0-only */

#include <cf9_reset.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <soc/intel/common/reset.h>

void do_global_reset(void)
{
	/* Ask CSE to do the global reset */
	if (cse_request_global_reset())
		return;

	/* global reset if CSE fail to reset */
	pmc_global_reset_enable(1);
	do_full_reset();
}
