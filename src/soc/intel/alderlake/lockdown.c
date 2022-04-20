/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 621483
 * Chapter number: 4
 */

#include <device/mmio.h>
#include <intelblocks/cfg.h>
#include <intelpch/lockdown.h>
#include <soc/pm.h>
#include <stdint.h>

static void pmc_lockdown_cfg(int chipset_lockdown)
{
	uint8_t *pmcbase = pmc_mmio_regs();

	/* PMSYNC */
	setbits32(pmcbase + PMSYNC_TPR_CFG, PCH2CPU_TPR_CFG_LOCK);
	/* Lock down ABASE and sleep stretching policy */
	setbits32(pmcbase + GEN_PMCON_B, SLP_STR_POL_LOCK | ACPI_BASE_LOCK);

	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT)
		setbits32(pmcbase + GEN_PMCON_B, SMI_LOCK);

	if (!CONFIG(USE_FSP_NOTIFY_PHASE_POST_PCI_ENUM)) {
		setbits32(pmcbase + ST_PG_FDIS1, ST_FDIS_LOCK);
		setbits32(pmcbase + SSML, SSML_SSL_EN);
		setbits32(pmcbase + PM_CFG, PM_CFG_DBG_MODE_LOCK |
					 PM_CFG_XRAM_READ_DISABLE);
	}
}

void soc_lockdown_config(int chipset_lockdown)
{
	/* PMC lock down configuration */
	pmc_lockdown_cfg(chipset_lockdown);
}
