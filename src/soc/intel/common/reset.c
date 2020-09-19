/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <halt.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <reset.h>

#include "reset.h"

static void force_global_reset(void)
{
	/* Ask CSE to do the global reset */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_CSE))
		if (cse_request_global_reset())
			return;

	/*
	 * If ME is unable to reset platform then enable the PMC CF9GR register [B0:D31:F2
	 * register offset 0xAC bit 20] and force a global reset by writing 0x06 or 0x0E.
	 */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_PMC))
		pmc_global_reset_enable(true);
	/* Now BIOS can write 0x06 or 0x0E to 0xCF9 port to global reset platform */
	do_full_reset();
}

void global_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	cf9_reset_prepare();
	dcache_clean_all();
	force_global_reset();
	halt();
}

void do_board_reset(void)
{
	full_reset();
}
