/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <intelblocks/cfg.h>
#include <intelblocks/pmclib.h>
#include <intelpch/lockdown.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>

#define SMM_FEATURE_CONTROL      0x8c
#define SMM_CODE_CHK_EN          (1 << 2)
#define SMM_FEATURE_CONTROL_LOCK (1 << 0)

static void pmc_lockdown_cfg(int chipset_lockdown)
{
	pmc_or_mmio32(PMSYNC_TPR_CFG, PCH2CPU_TPR_CFG_LOCK);
	pmc_or_mmio32(GEN_PMCON_B, SLP_STR_POL_LOCK | ACPI_BASE_LOCK);

	pmc_global_reset_disable_and_lock();

	assert(chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT);

	pmc_or_mmio32(GEN_PMCON_B, SMI_LOCK);
}

void soc_lockdown_config(int chipset_lockdown)
{
	pmc_lockdown_cfg(chipset_lockdown);

	pci_or_config32(UBOX_DEV_RACU, SMM_FEATURE_CONTROL,
			SMM_CODE_CHK_EN | SMM_FEATURE_CONTROL_LOCK);
}
