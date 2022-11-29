/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelpch/lockdown.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <stdint.h>

/* PCR PSTH Control Register */
#define PCR_PSTH_CTRLREG	0x1d00
#define PSTH_CTRLREG_IOSFPTCGE	(1 << 2)

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
		setbits32(pmcbase + GEN_PMCON_B, ST_FDIS_LOCK);
		setbits32(pmcbase + SSML, SSML_SSL_EN);
		setbits32(pmcbase + PM_CFG, PM_CFG_DBG_MODE_LOCK |
					 PM_CFG_XRAM_READ_DISABLE);
	}

	/* Send PMC IPC to inform about PCI enumeration done */
	pmc_send_pci_enum_done();
}

static void soc_die_lockdown_cfg(void)
{
	if (CONFIG(USE_FSP_NOTIFY_PHASE_POST_PCI_ENUM))
		return;

	/* Enable IOSF Primary Trunk Clock Gating */
	pcr_rmw32(PID_PSTH, PCR_PSTH_CTRLREG, ~0, PSTH_CTRLREG_IOSFPTCGE);
}

void soc_lockdown_config(int chipset_lockdown)
{
	/* PMC lock down configuration */
	pmc_lockdown_cfg(chipset_lockdown);
	/* SOC Die lock down configuration */
	soc_die_lockdown_cfg();
}
