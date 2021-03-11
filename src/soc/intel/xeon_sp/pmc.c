/* SPDX-License-Identifier: GPL-2.0-only */

#include "chip.h"
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <reg_script.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

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

#if ENV_RAMSTAGE
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
#endif

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

void soc_get_gpi_gpe_configs(uint8_t *dw0, uint8_t *dw1, uint8_t *dw2)
{
	/* No functionality for this yet */
}

int rtc_failure(void)
{
	u8 reg8;
	int rtc_failed;
	/* PMC Controller Device 0x1F, Func 02 */
	reg8 = pci_read_config8(PCH_DEV_PMC, GEN_PMCON_B);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~RTC_BATTERY_DEAD;
		pci_write_config8(PCH_DEV_PMC, GEN_PMCON_B, reg8);
		printk(BIOS_DEBUG, "rtc_failed = 0x%x\n", rtc_failed);
	}

	return !!rtc_failed;
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

void pmc_lock_smi(void)
{
	printk(BIOS_DEBUG, "Locking SMM enable.\n");
	pci_or_config32(PCH_DEV_PMC, GEN_PMCON_A, SMI_LOCK);
}

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
