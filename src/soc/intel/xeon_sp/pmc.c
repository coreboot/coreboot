/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <reg_script.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

#include "chip.h"

/* Fill up PMC resource structure */
int pmc_soc_get_resources(struct pmc_resource_config *cfg)
{
	cfg->pwrmbase_offset = PWRMBASE;
	cfg->pwrmbase_addr = PCH_PWRM_BASE_ADDRESS;
	cfg->pwrmbase_size = PCH_PWRM_BASE_SIZE;
	cfg->abase_offset = ABASE;
	cfg->abase_addr = ACPI_BASE_ADDRESS;
	cfg->abase_size = ACPI_BASE_SIZE;

	return 0;
}

static const struct reg_script pch_pmc_misc_init_script[] = {
	/* Enable SCI and clear SLP requests. */
	REG_IO_RMW32(ACPI_BASE_ADDRESS + PM1_CNT, ~SLP_TYP, SCI_EN),
	REG_SCRIPT_END
};

static const struct reg_script pmc_write1_to_clear_script[] = {
	REG_PCI_OR32(GEN_PMCON_A, 0),
	REG_PCI_OR32(GEN_PMCON_B, 0),
	REG_PCI_OR32(GEN_PMCON_B, 0),
	REG_RES_OR32(PWRMBASE, GBLRST_CAUSE0, 0),
	REG_RES_OR32(PWRMBASE, GBLRST_CAUSE1, 0),
	REG_SCRIPT_END
};

void pmc_soc_init(struct device *dev)
{
	pmc_set_power_failure_state(true);
	pmc_gpe_init();

	/* Note that certain bits may be cleared from running script as
	 * certain bit fields are write 1 to clear. */
	reg_script_run_on_dev(dev, pch_pmc_misc_init_script);
	pmc_set_acpi_mode();

	/* Clear registers that contain write-1-to-clear bits. */
	reg_script_run_on_dev(dev, pmc_write1_to_clear_script);
}

void pmc_lock_smi(void)
{
	printk(BIOS_DEBUG, "Locking SMM enable.\n");
	pci_or_config32(PCH_DEV_PMC, GEN_PMCON_A, SMI_LOCK);
}
