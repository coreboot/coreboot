/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/smm_reloc.h>
#include <device/mmio.h>
#include <intelblocks/oc_wdt.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/systemagent.h>
#include <soc/pm.h>
#include <soc/pmc.h>

void smm_southbridge_clear_state(void)
{
	printk(BIOS_DEBUG, "Clearing SMI status registers\n");

	if (pmc_get_smi_en() & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	/* Dump and clear status registers */
	pmc_clear_smi_status();
	pmc_clear_pm1_status();
	pmc_clear_tco_status();
	pmc_clear_all_gpe_status();
}

static void configure_periodic_smi_interval(void)
{
	uint32_t gen_pmcon;
	uint32_t gen_pmcon_reg;

	if (CONFIG(PERIODIC_SMI_RATE_SELECTION_IN_GEN_PMCON_B))
		gen_pmcon_reg = GEN_PMCON_B;
	else
		gen_pmcon_reg = GEN_PMCON_A;

	/*
	 * Periodic SMIs have +/- 1 second error, to be safe add few seconds
	 * more. Also we do not allow timeouts lower than 70s by Kconfig
	 * definition, so we need to handle one case.
	 */
	gen_pmcon = read32p(soc_read_pmc_base() + gen_pmcon_reg);
	gen_pmcon &= ~PER_SMI_SEL_MASK;
	gen_pmcon |= SMI_RATE_64S;
	write32p(soc_read_pmc_base() + gen_pmcon_reg, gen_pmcon);

	/*
	 * We don't know when SMI timer is started or even if this is
	 * architecturally defined, but in worst case we may get SMI 64
	 * seconds (+ any error) from now, which may be more than OC watchdog
	 * timeout since it was last kicked, so we should kick it here, just
	 * in case.
	 */
	oc_wdt_reload();
}

static void smm_southbridge_enable(uint16_t pm1_events)
{
	uint32_t smi_params = ENABLE_SMI_PARAMS;

	printk(BIOS_DEBUG, "Enabling SMIs.\n");
	/* Configure events */
	pmc_enable_pm1(pm1_events);
	pmc_disable_std_gpe(PME_B0_EN);

	/*
	 * GPEs need to be disabled before enabling SMI. Otherwise, it could
	 * lead to SMIs being triggered in coreboot preventing the progress of
	 * normal boot-up. This is done as late as possible so that
	 * pmc_fill_power_state can read the correct state of GPE0_EN* registers
	 * and not lose information about the wake source.
	 */
	pmc_disable_all_gpe();

	/*
	 * Enable SMI generation:
	 *  - on APMC writes (io 0xb2)
	 *  - on writes to SLP_EN (sleep states)
	 *  - on writes to GBL_RLS (bios commands)
	 *  - on eSPI events, unless disabled (does nothing on LPC systems)
	 *  - on TCO events (TIMEOUT, case intrusion, ...), if enabled
	 *  - periodically, if watchdog feeding through SMI is enabled
	 * No SMIs:
	 *  - on microcontroller writes (io 0x62/0x66)
	 */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_ESPI_DISABLE))
		smi_params &= ~ESPI_SMI_EN;

	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE))
		smi_params |= TCO_SMI_EN;

	if (CONFIG(SOC_INTEL_COMMON_OC_WDT_RELOAD_IN_PERIODIC_SMI)) {
		smi_params |= PERIODIC_EN;
		configure_periodic_smi_interval();
	}

	/* Enable SMI generation: */
	pmc_enable_smi(smi_params);
}

void global_smi_enable(void)
{
	smm_southbridge_enable(PWRBTN_EN | GBL_EN);
}

void global_smi_enable_no_pwrbtn(void)
{
	smm_southbridge_enable(GBL_EN);
}
