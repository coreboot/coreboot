/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Helper functions for dealing with power management registers
 * and the differences between PCH variants.
 */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <device/pci.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <soc/pm.h>
#include <soc/pmc.h>

uint8_t *pmc_mmio_regs(void)
{
	/* PMC PWRMBASE can be hidden after FSP-S so can't be accessed using PCI CSR,
	 * hence use hard-coded address */
	return (void *)(uintptr_t)PCH_PWRM_BASE_ADDRESS;
}

uintptr_t soc_read_pmc_base(void)
{
	return (uintptr_t)(pmc_mmio_regs());
}

uint32_t *soc_pmc_etr_addr(void)
{
	/*
	 * The pointer returned must not be cached, because the address depends on the
	 * MMCONF base address and the assigned PCI bus number, which both may change
	 * during the boot process!
	 */
	return (uint32_t *)(soc_read_pmc_base() + ETR);
}

int soc_get_rtc_failed(void)
{
	uint32_t pmcon_b = read32(pmc_mmio_regs() + GEN_PMCON_B);
	int rtc_fail = !!(pmcon_b & RTC_BATTERY_DEAD);

	if (rtc_fail)
		printk(BIOS_ERR, "%s: RTC battery dead or removed\n", __func__);

	return rtc_fail;
}

/*
 * Set which power state system will be after reapplying
 * the power (from G3 State)
 */
void pmc_soc_set_afterg3_en(const bool on)
{
	uint8_t reg8;
	uint8_t *const pmcbase = pmc_mmio_regs();

	reg8 = read8(pmcbase + GEN_PMCON_A);
	if (on)
		reg8 &= ~SLEEP_AFTER_POWER_FAIL;
	else
		reg8 |= SLEEP_AFTER_POWER_FAIL;
	write8(pmcbase + GEN_PMCON_A, reg8);
}

void pmc_lockdown_config(void)
{
	/* Make sure payload/OS can't trigger global reset */
	pmc_global_reset_disable_and_lock();

	/* Lock PMC stretch policy */
	pmc_or_mmio32(GEN_PMCON_B, SLP_STR_POL_LOCK);
	pmc_or_mmio32(PM_CFG, PMC_LOCK);
}
