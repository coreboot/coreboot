/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/pmclib.h>
#include <intelpch/lockdown.h>
#include <soc/pm.h>

void soc_lockdown_config(int chipset_lockdown)
{
	/*
	 * Nothing to do here as pmc_global_reset_disable_and_lock
	 * is called from chip.c
	 */
}
