/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Helper functions for dealing with power management registers
 * and the differences between PCH variants.
 */

#define __SIMPLE_DEVICE__

#include <acpi/acpi.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/tco.h>
#include <soc/gpe.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/smbus.h>
#include <security/vboot/vbnv.h>

#include "chip.h"

/*
 * SMI
 */

const char *const *soc_smi_sts_array(size_t *smi_arr)
{
	static const char *const smi_sts_bits[] = {
		[BIOS_STS_BIT] = "BIOS",
		[LEGACY_USB_STS_BIT] = "LEGACY_USB",
		[SMI_ON_SLP_EN_STS_BIT] = "SLP_SMI",
		[APM_STS_BIT] = "APM",
		[SWSMI_TMR_STS_BIT] = "SWSMI_TMR",
		[PM1_STS_BIT] = "PM1",
		[GPE0_STS_BIT] = "GPE0",
		[GPIO_STS_BIT] = "GPI",
		[MCSMI_STS_BIT] = "MCSMI",
		[DEVMON_STS_BIT] = "DEVMON",
		[TCO_STS_BIT] = "TCO",
		[PERIODIC_STS_BIT] = "PERIODIC",
		[SERIRQ_SMI_STS_BIT] = "SERIRQ_SMI",
		[SMBUS_SMI_STS_BIT] = "SMBUS_SMI",
		[PCI_EXP_SMI_STS_BIT] = "PCI_EXP_SMI",
		[MONITOR_STS_BIT] = "MONITOR",
		[SPI_SMI_STS_BIT] = "SPI",
		[GPIO_UNLOCK_SMI_STS_BIT] = "GPIO_UNLOCK",
		[ESPI_SMI_STS_BIT] = "ESPI_SMI",
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
		[1] = "SW_TCO",
		[2] = "TCO_INT",
		[3] = "TIMEOUT",
		[7] = "NEWCENTURY",
		[8] = "BIOSWR",
		[9] = "DMISCI",
		[10] = "DMISMI",
		[12] = "DMISERR",
		[13] = "SLVSEL",
		[16] = "INTRD_DET",
		[17] = "SECOND_TO",
		[18] = "BOOT",
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
		[1] = "HOTPLUG",
		[2] = "SWGPE",
		[6] = "TCO_SCI",
		[7] = "SMB_WAK",
		[9] = "PCI_EXP",
		[10] = "BATLOW",
		[11] = "PME",
		[12] = "ME",
		[13] = "PME_B0",
		[14] = "eSPI",
		[15] = "GPIO Tier-2",
		[16] = "LAN_WAKE",
		[18] = "WADT"
	};

	*gpe_arr = ARRAY_SIZE(gpe_sts_bits);
	return gpe_sts_bits;
}

void pmc_set_disb(void)
{
	/* Set the DISB after DRAM init */
	u32 disb_val;
	const pci_devfn_t dev = PCH_DEV_PMC;

	disb_val = pci_read_config32(dev, GEN_PMCON_A);
	disb_val |= DISB;

	/* Don't clear bits that are write-1-to-clear */
	disb_val &= ~(GBL_RST_STS | MS4V);
	pci_write_config32(dev, GEN_PMCON_A, disb_val);
}

uint8_t *pmc_mmio_regs(void)
{
	uint32_t reg32;

	reg32 = pci_read_config32(PCH_DEV_PMC, PWRMBASE);

	/* 4KiB alignment. */
	reg32 &= ~0xfff;

	return (void *)(uintptr_t) reg32;
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
	DEVTREE_CONST struct soc_intel_skylake_config *config;

	config = config_of_soc();

	/* Assign to out variable */
	*dw0 = config->gpe0_dw0;
	*dw1 = config->gpe0_dw1;
	*dw2 = config->gpe0_dw2;
}

int rtc_failure(void)
{
	u8 reg8;
	int rtc_failed;
	/* PMC Controller Device 0x1F, Func 02 */
	const pci_devfn_t dev = PCH_DEV_PMC;

	reg8 = pci_read_config8(dev, GEN_PMCON_B);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~RTC_BATTERY_DEAD;
		pci_write_config8(dev, GEN_PMCON_B, reg8);
		printk(BIOS_DEBUG, "rtc_failed = 0x%x\n", rtc_failed);
	}

	return !!rtc_failed;
}

int vbnv_cmos_failed(void)
{
	return rtc_failure();
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

	/*
	 * If waking from S3 determine if deep S3 is enabled. If not,
	 * need to check both deep sleep well and normal suspend well.
	 * Otherwise just check deep sleep well.
	 */
	if (prev_sleep_state == ACPI_S3) {
		/* PWR_FLR represents deep sleep power well loss. */
		uint32_t mask = PWR_FLR;

		/* If deep s3 isn't enabled check the suspend well too. */
		if (!deep_s3_enabled())
			mask |= SUS_PWR_FLR;

		if (ps->gen_pmcon_b & mask)
			prev_sleep_state = ACPI_S5;
	}
	return prev_sleep_state;
}

void soc_fill_power_state(struct chipset_power_state *ps)
{
	uint8_t *pmc;

	ps->tco1_sts = tco_read_reg(TCO1_STS);
	ps->tco2_sts = tco_read_reg(TCO2_STS);

	printk(BIOS_DEBUG, "TCO_STS:   %04x %04x\n", ps->tco1_sts, ps->tco2_sts);

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
	const pci_devfn_t dev = PCH_DEV_PMC;

	reg8 = pci_read_config8(dev, GEN_PMCON_B);
	if (on)
		reg8 &= ~SLEEP_AFTER_POWER_FAIL;
	else
		reg8 |= SLEEP_AFTER_POWER_FAIL;
	pci_write_config8(dev, GEN_PMCON_B, reg8);
}
