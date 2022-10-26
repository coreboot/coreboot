/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci.h>
#include <intelblocks/cfg.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pmclib.h>
#include <intelpch/lockdown.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

static void lpc_lockdown_config(void)
{
	/* Set BIOS Interface Lock, BIOS Lock */
	lpc_set_bios_interface_lock_down();

	/* Only allow writes in SMM */
	if (CONFIG(BOOTMEDIA_SMM_BWP)) {
		lpc_set_eiss();
		lpc_enable_wp();
	}
	lpc_set_lock_enable();
}

static void pmc_lockdown_config(int chipset_lockdown)
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

	/* Lock PMC stretch policy */
	pci_or_config32(PCH_DEV_PMC, GEN_PMCON_B, SLP_STR_POL_LOCK);
}

static void sata_lockdown_config(int chipset_lockdown)
{
	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		pci_or_config32(PCH_DEV_SATA, SATAGC, SATAGC_REGLOCK);
		pci_or_config32(PCH_DEV_SSATA, SATAGC, SATAGC_REGLOCK);
	}
}

void soc_lockdown_config(int chipset_lockdown)
{
	lpc_lockdown_config();
	pmc_lockdown_config(chipset_lockdown);
	sata_lockdown_config(chipset_lockdown);
}
