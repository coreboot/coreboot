/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/pmclib.h>
#include <intelpch/lockdown.h>
#include <soc/pm.h>

static void pmc_lock_smi(void)
{
	uint8_t *pmcbase;

	pmcbase = pmc_mmio_regs();

	setbits32(pmcbase + GEN_PMCON2, SMI_LOCK);
}

void soc_lockdown_config(int chipset_lockdown)
{
	/* Only Gemini Lake exposes SMI_LOCK through the PMC MMIO path. */
	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT &&
	    CONFIG(SOC_INTEL_GEMINILAKE))
		pmc_lock_smi();
}
