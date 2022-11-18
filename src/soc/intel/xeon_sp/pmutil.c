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
#include <types.h>

/*
 * SMI
 */

const char *const *soc_smi_sts_array(size_t *smi_arr)
{
	static const char *const smi_sts_bits[] = {
		[2] = "BIOS",
		[3] = "LEGACY_USB",
		[4] = "SLP_SMI",
		[5] = "APM",
		[6] = "SWSMI_TMR",
		[7] = "BIOS_RLS",
		[8] = "PM1",
		[9] = "GPE0",
		[10] = "GPI",
		[11] = "MCSMI",
		[12] = "DEVMON",
		[13] = "TCO",
		[14] = "PERIODIC",
		[20] = "PCI_EXP_SMI",
		[23] = "IE_SMI",
		[25] = "SCC_SMI",
		[26] = "SPI",
		[27] = "GPIO_UNLOCK",
		[28] = "ESPI_SMI",
		[29] = "SERIAL_I/O",
		[30] = "ME_SMI",
		[31] = "XHCI",
	};

	*smi_arr = ARRAY_SIZE(smi_sts_bits);
	return smi_sts_bits;
}

/*
 * TCO
 */

const char *const *soc_tco_sts_array(size_t *tco_arr)
{
	static const char *const tco_sts_bits[] = {
		[0] = "NMI2SMI",
		[1] = "OS_TCO",
		[2] = "TCO_INT",
		[3] = "TIMEOUT",
		[7] = "NEWCENTURY",
		[8] = "BIOSWR",
		[9] = "CPUSCI",
		[10] = "CPUSMI",
		[12] = "CPUSERR",
		[13] = "SLVSEL",
		[16] = "INTRD_DET",
		[17] = "SECOND_TO",
		[20] = "SMLINK_SLV"
	};

	*tco_arr = ARRAY_SIZE(tco_sts_bits);
	return tco_sts_bits;
}

/*
 * GPE0
 */

const char *const *soc_std_gpe_sts_array(size_t *gpe_arr)
{
	static const char *const gpe_sts_bits[] = {
	};

	*gpe_arr = ARRAY_SIZE(gpe_sts_bits);
	return gpe_sts_bits;
}

uint8_t *pmc_mmio_regs(void)
{
	return (void *)(uintptr_t)pci_read_config32(PCH_DEV_PMC, PWRMBASE);
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
	return pci_mmio_config32_addr(PCH_DEVFN_PMC << 12, ETR);
}

void soc_get_gpi_gpe_configs(uint8_t *dw0, uint8_t *dw1, uint8_t *dw2)
{
	/* No functionality for this yet */
}

int soc_get_rtc_failed(void)
{
	uint32_t pmcon_b = pci_s_read_config32(PCH_DEV_PMC, GEN_PMCON_B);
	int rtc_fail = !!(pmcon_b & RTC_BATTERY_DEAD);

	if (rtc_fail)
		printk(BIOS_ERR, "%s: RTC battery dead or removed\n", __func__);

	return rtc_fail;
}

/* Return 0, 3, or 5 to indicate the previous sleep state. */
int soc_prev_sleep_state(const struct chipset_power_state *ps, int prev_sleep_state)
{
	/*
	 * Check for any power failure to determine if this a wake from
	 * S5 because the PCH does not set the WAK_STS bit when waking
	 * from a true G3 state.
	 */
	if (!(ps->pm1_sts & WAK_STS) &&
	    (ps->gen_pmcon_b & (PWR_FLR | SUS_PWR_FLR)))
		prev_sleep_state = ACPI_S5;

	return prev_sleep_state;
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

/* STM Support */
uint16_t get_pmbase(void)
{
	return ACPI_BASE_ADDRESS;
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
