/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <bootstate.h>
#include <console/console.h>
#include <console/post_codes.h>
#include <cpu/x86/smm.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <intelblocks/tco.h>
#include <intelblocks/thermal.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <spi-generic.h>

static void pch_finalize(void)
{
	uint32_t reg32;
	uint8_t *pmcbase;
	config_t *config;
	uint8_t reg8;

	/* TCO Lock down */
	tco_lockdown();

	/* TODO: Add Thermal Configuration */

	/*
	 * Disable ACPI PM timer based on dt policy
	 *
	 * Disabling ACPI PM timer is necessary for XTAL OSC shutdown.
	 * Disabling ACPI PM timer also switches off TCO
	 *
	 * SA_DEV_ROOT device is used here instead of PCH_DEV_PMC since it is
	 * just required to get to chip config. PCH_DEV_PMC is hidden by this
	 * point and hence removed from the root bus. pcidev_path_on_root thus
	 * returns NULL for PCH_DEV_PMC device.
	 */
	config = config_of_soc();
	pmcbase = pmc_mmio_regs();
	if (config->PmTimerDisabled) {
		reg8 = read8(pmcbase + PCH_PWRM_ACPI_TMR_CTL);
		reg8 |= (1 << 1);
		write8(pmcbase + PCH_PWRM_ACPI_TMR_CTL, reg8);
	}

	/* Disable XTAL shutdown qualification for low power idle. */
	if (config->s0ix_enable) {
		reg32 = read32(pmcbase + CPPMVRIC);
		reg32 |= XTALSDQDIS;
		write32(pmcbase + CPPMVRIC, reg32);
	}

	pmc_clear_pmcon_sts();
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize();
	apm_control(APM_CNT_FINALIZE);

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
