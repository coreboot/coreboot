/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pmclib.h>
#include <intelpch/lockdown.h>
#include <soc/pm.h>

static void lpc_lockdown_config(int chipset_lockdown)
{
	/* Set BIOS Interface Lock, BIOS Lock */
	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		lpc_set_bios_interface_lock_down();
		lpc_set_lock_enable();
	}
}

static void pmc_lockdown_config(void)
{
	uint8_t *pmcbase;
	u32 pmsyncreg;

	/* PMSYNC */
	pmcbase = pmc_mmio_regs();
	pmsyncreg = read32(pmcbase + PMSYNC_TPR_CFG);
	pmsyncreg |= PMSYNC_LOCK;
	write32(pmcbase + PMSYNC_TPR_CFG, pmsyncreg);

	/* Make sure payload/OS can't trigger global reset */
	pmc_global_reset_disable_and_lock();
}

void soc_lockdown_config(int chipset_lockdown)
{
	/* LPC lock down configuration */
	lpc_lockdown_config(chipset_lockdown);

	/* PMC lock down configuration */
	pmc_lockdown_config();
}
