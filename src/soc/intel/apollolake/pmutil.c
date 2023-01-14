/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <acpi/acpi.h>
#include <acpi/acpi_pm.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <intelblocks/msr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <intelblocks/tco.h>
#include <soc/iomap.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <security/vboot/vbnv.h>

#include "chip.h"

uint8_t *pmc_mmio_regs(void)
{
	return (void *)(uintptr_t)PCH_PWRM_BASE_ADDRESS;
}

uintptr_t soc_read_pmc_base(void)
{
	return (uintptr_t)pmc_mmio_regs();
}

uint32_t *soc_pmc_etr_addr(void)
{
	return (uint32_t *)(soc_read_pmc_base() + ETR);
}

const char *const *soc_smi_sts_array(size_t *a)
{
	static const char *const smi_sts_bits[] = {
		[BIOS_STS_BIT] = "BIOS",
		[LEGACY_USB_STS_BIT] = "LEGACY USB",
		[SMI_ON_SLP_EN_STS_BIT] = "SLP_SMI",
		[APM_STS_BIT] = "APM",
		[SWSMI_TMR_STS_BIT] = "SWSMI_TMR",
		[PM1_STS_BIT] = "PM1",
		[GPE0_STS_BIT] = "GPE0 (reserved)",
		[GPIO_STS_BIT] = "GPIO_SMI",
		[GPIO_UNLOCK_SMI_STS_BIT] = "GPIO_UNLOCK_SSMI",
		[MC_SMI_STS_BIT] = "MCSMI",
		[TCO_STS_BIT] = "TCO",
		[PERIODIC_STS_BIT] = "PERIODIC",
		[SERIRQ_SMI_STS_BIT] = "SERIRQ",
		[SMBUS_SMI_STS_BIT] = "SMBUS_SMI",
		[XHCI_SMI_STS_BIT] = "XHCI",
		[HSMBUS_SMI_STS_BIT] = "HOST_SMBUS",
		[SCS_SMI_STS_BIT] = "SCS",
		[PCI_EXP_SMI_STS_BIT] = "PCI_EXP_SMI",
		[SCC2_SMI_STS_BIT] = "SCC2",
		[SPI_SSMI_STS_BIT] = "SPI_SSMI",
		[SPI_SMI_STS_BIT] = "SPI",
		[PMC_OCP_SMI_STS_BIT] = "OCP_CSE",
	};

	*a = ARRAY_SIZE(smi_sts_bits);
	return smi_sts_bits;
}

/*
 * For APL/GLK this check for power button status if nothing else
 * is indicating an SMI and SMIs aren't turned into SCIs.
 * Apparently, there is no PM1 status bit in the SMI status
 * register.  That makes things difficult for
 * determining if the power button caused an SMI.
 */
uint32_t soc_get_smi_status(uint32_t generic_sts)
{
	if (generic_sts == 0 && !(pmc_read_pm1_control() & SCI_EN)) {
		uint16_t pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);

		/* Fake PM1 status bit if power button pressed. */
		if (pm1_sts & PWRBTN_STS)
			generic_sts |= (1 << PM1_STS_BIT);
	}

	/*
	 * GPE0_STS is reserved in APL/GLK datasheets. For compatibility
	 * with common code, mask it out so that it is always zero.
	 */
	return generic_sts & ~(1 << GPE0_STS_BIT);
}

const char *const *soc_tco_sts_array(size_t *a)
{
	static const char *const tco_sts_bits[] = {
		[3] = "TIMEOUT",
		[17] = "SECOND_TO",
	};

	*a = ARRAY_SIZE(tco_sts_bits);
	return tco_sts_bits;
}

const char *const *soc_std_gpe_sts_array(size_t *a)
{
	static const char *const gpe_sts_bits[] = {
		[0] = "PCIE_SCI",
		[2] = "SWGPE",
		[3] = "PCIE_WAKE0",
		[4] = "PUNIT",
		[6] = "PCIE_WAKE1",
		[7] = "PCIE_WAKE2",
		[8] = "PCIE_WAKE3",
		[9] = "PCI_EXP",
		[10] = "BATLOW",
		[11] = "CSE_PME",
		[12] = "XDCI_PME",
		[13] = "XHCI_PME",
		[14] = "AVS_PME",
		[15] = "GPIO_TIER1_SCI",
		[16] = "SMB_WAK",
		[17] = "SATA_PME",
	};

	*a = ARRAY_SIZE(gpe_sts_bits);
	return gpe_sts_bits;
}

void soc_clear_pm_registers(uintptr_t pmc_bar)
{
	uint32_t gen_pmcon1;

	gen_pmcon1 = read32p(pmc_bar + GEN_PMCON1);
	/* Clear the status bits. The RPS field is cleared on a 0 write. */
	write32p(pmc_bar + GEN_PMCON1, gen_pmcon1 & ~RPS);
}

void soc_get_gpi_gpe_configs(uint8_t *dw0, uint8_t *dw1, uint8_t *dw2)
{
	DEVTREE_CONST struct soc_intel_apollolake_config *config;

	config = config_of_soc();

	/* Assign to out variable */
	*dw0 = config->gpe0_dw1;
	*dw1 = config->gpe0_dw2;
	*dw2 = config->gpe0_dw3;
}

void soc_fill_power_state(struct chipset_power_state *ps)
{
	uintptr_t pmc_bar0 = soc_read_pmc_base();

	ps->tco1_sts = tco_read_reg(TCO1_STS);
	ps->tco2_sts = tco_read_reg(TCO2_STS);

	ps->prsts = read32p(pmc_bar0 + PRSTS);
	ps->gen_pmcon1 = read32p(pmc_bar0 + GEN_PMCON1);
	ps->gen_pmcon2 = read32p(pmc_bar0 + GEN_PMCON2);
	ps->gen_pmcon3 = read32p(pmc_bar0 + GEN_PMCON3);

	printk(BIOS_DEBUG, "prsts: %08x\n",
	       ps->prsts);
	printk(BIOS_DEBUG, "tco_sts:   %04x %04x\n",
	       ps->tco1_sts, ps->tco2_sts);
	printk(BIOS_DEBUG,
	       "gen_pmcon1: %08x gen_pmcon2: %08x gen_pmcon3: %08x\n",
	       ps->gen_pmcon1, ps->gen_pmcon2, ps->gen_pmcon3);
}

/* Return 0, 3, or 5 to indicate the previous sleep state. */
int soc_prev_sleep_state(const struct chipset_power_state *ps,
	int prev_sleep_state)
{
	/* WAK_STS bit will not be set when waking from G3 state */

	if (!(ps->pm1_sts & WAK_STS) && (ps->gen_pmcon1 & COLD_BOOT_STS))
		prev_sleep_state = ACPI_S5;
	return prev_sleep_state;
}

static int rtc_failed(uint32_t gen_pmcon1)
{
	return !!(gen_pmcon1 & RPS);
}

int soc_get_rtc_failed(void)
{
	const struct chipset_power_state *ps;

	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_RTC) < 0)
		return 1;

	return rtc_failed(ps->gen_pmcon1);
}

int vbnv_cmos_failed(void)
{
	uintptr_t pmc_bar = soc_read_pmc_base();
	uint32_t gen_pmcon1 = read32p(pmc_bar + GEN_PMCON1);
	int rtc_failure = rtc_failed(gen_pmcon1);

	if (rtc_failure) {
		printk(BIOS_INFO, "RTC failed!\n");

		/* We do not want to write 1 to clear-1 bits. Set them to 0. */
		gen_pmcon1 &= ~GEN_PMCON1_CLR1_BITS;

		/* RPS is write 0 to clear. */
		gen_pmcon1 &= ~RPS;

		write32p(pmc_bar + GEN_PMCON1, gen_pmcon1);
	}

	return rtc_failure;
}

/* STM Support */
uint16_t get_pmbase(void)
{
	return (uint16_t)ACPI_BASE_ADDRESS;
}

void pmc_soc_set_afterg3_en(const bool on)
{
	const uintptr_t gen_pmcon1 = soc_read_pmc_base() + GEN_PMCON1;
	uint32_t reg32;

	reg32 = read32p(gen_pmcon1);
	if (on)
		reg32 &= ~SLEEP_AFTER_POWER_FAIL;
	else
		reg32 |= SLEEP_AFTER_POWER_FAIL;
	write32p(gen_pmcon1, reg32);
}
