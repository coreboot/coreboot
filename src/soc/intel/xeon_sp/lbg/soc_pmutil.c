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
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>

uint8_t *pmc_mmio_regs(void)
{
	return (void *)(uintptr_t) pci_read_config32(PCH_DEV_PMC, PWRMBASE);
}

uintptr_t soc_read_pmc_base(void)
{
	return (uintptr_t) (pmc_mmio_regs());
}

uint32_t *soc_pmc_etr_addr(void)
{
	/*
	 * The pointer returned must not be cached, because the address depends on the
	 * MMCONF base address and the assigned PCI bus number, which both may change
	 * during the boot process!
	 */
	return pci_mmio_config32_addr(PCH_DEVFN_PMC << 12, ETR);
}

int soc_get_rtc_failed(void)
{
	uint32_t pmcon_b = pci_s_read_config32(PCH_DEV_PMC, GEN_PMCON_B);
	int rtc_fail = !!(pmcon_b & RTC_BATTERY_DEAD);

	if (rtc_fail)
		printk(BIOS_ERR, "%s: RTC battery dead or removed\n", __func__);

	return rtc_fail;
}

void soc_fill_power_state(struct chipset_power_state *ps)
{
	uint8_t *pmc;

	ps->gen_pmcon_a = pci_read_config32(PCH_DEV_PMC, GEN_PMCON_A);
	ps->gen_pmcon_b = pci_read_config32(PCH_DEV_PMC, GEN_PMCON_B);

	pmc = pmc_mmio_regs();
	ps->gblrst_cause[0] = read32(pmc + GBLRST_CAUSE0);
	ps->gblrst_cause[1] = read32(pmc + GBLRST_CAUSE1);

	printk(BIOS_DEBUG, "GEN_PMCON: %08x %08x\n",
	       ps->gen_pmcon_a, ps->gen_pmcon_b);

	printk(BIOS_DEBUG, "GBLRST_CAUSE: %08x %08x\n",
	       ps->gblrst_cause[0], ps->gblrst_cause[1]);
}

/*
 * Set which power state system will be after reapplying
 * the power (from G3 State)
 */
void pmc_soc_set_afterg3_en(const bool on)
{
	uint8_t reg8;

	reg8 = pci_read_config8(PCH_DEV_PMC, GEN_PMCON_B);
	if (on)
		reg8 &= ~SLEEP_AFTER_POWER_FAIL;
	else
		reg8 |= SLEEP_AFTER_POWER_FAIL;
	pci_write_config8(PCH_DEV_PMC, GEN_PMCON_B, reg8);
}
