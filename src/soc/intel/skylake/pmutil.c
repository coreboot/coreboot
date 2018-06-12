/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Helper functions for dealing with power management registers
 * and the differences between PCH variants.
 */

#include <arch/acpi.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <intelblocks/pmclib.h>
#include <halt.h>
#include <intelblocks/lpc_lib.h>
#include <rules.h>
#include <stdlib.h>
#include <soc/gpe.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/smbus.h>
#include <timer.h>
#include <security/vboot/vbnv.h>
#include "chip.h"

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
		[8] = "PM1",
		[9] = "GPE0",
		[10] = "GPI",
		[11] = "MCSMI",
		[12] = "DEVMON",
		[13] = "TCO",
		[14] = "PERIODIC",
		[15] = "SERIRQ_SMI",
		[16] = "SMBUS_SMI",
		[17] = "LEGACY_USB2",
		[18] = "INTEL_USB2",
		[20] = "PCI_EXP_SMI",
		[21] = "MONITOR",
		[26] = "SPI",
		[27] = "GPIO_UNLOCK",
		[28] = "ESPI_SMI",
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

int acpi_sci_irq(void)
{
	int scis = pci_read_config32(PCH_DEV_PMC, ACTL) & SCI_IRQ_SEL;
	int sci_irq = 9;

	/* Determine how SCI is routed. */
	switch (scis) {
	case SCIS_IRQ9:
	case SCIS_IRQ10:
	case SCIS_IRQ11:
		sci_irq = scis - SCIS_IRQ9 + 9;
		break;
	case SCIS_IRQ20:
	case SCIS_IRQ21:
	case SCIS_IRQ22:
	case SCIS_IRQ23:
		sci_irq = scis - SCIS_IRQ20 + 20;
		break;
	default:
		printk(BIOS_DEBUG, "Invalid SCI route! Defaulting to IRQ9.\n");
		sci_irq = 9;
		break;
	}

	printk(BIOS_DEBUG, "SCI is IRQ%d\n", sci_irq);
	return sci_irq;
}

uint8_t *pmc_mmio_regs(void)
{
	uint32_t reg32;

	reg32 = pci_read_config32(PCH_DEV_PMC, PWRMBASE);

	/* 4KiB alignment. */
	reg32 &= ~0xfff;

	return (void *)(uintptr_t) reg32;
}

uint16_t smbus_tco_regs(void)
{
	uint16_t reg16;

	reg16 = pci_read_config16(PCH_DEV_SMBUS, TCOBASE);

	reg16 &= ~0x1f;

	return reg16;
}

uint32_t soc_reset_tco_status(void)
{
	u16 tco1_sts;
	u16 tco2_sts;
	u16 tcobase;

	tcobase = smbus_tco_regs();

	/* TCO Status 2 register */
	tco2_sts = inw(tcobase + TCO2_STS);
	tco2_sts |= (TCO2_STS_SECOND_TO | TCO2_STS_BOOT);
	outw(tco2_sts, tcobase + TCO2_STS);

	/* TCO Status 1 register */
	tco1_sts = inw(tcobase + TCO1_STS);

	/* Clear SECOND_TO_STS bit */
	if (tco2_sts & TCO2_STS_SECOND_TO)
		outw(tco2_sts & ~TCO2_STS_SECOND_TO, tcobase + TCO2_STS);

	return (tco2_sts << 16) | tco1_sts;
}

uintptr_t soc_read_pmc_base(void)
{
	return (uintptr_t) (pmc_mmio_regs());
}

void soc_get_gpi_gpe_configs(uint8_t *dw0, uint8_t *dw1, uint8_t *dw2)
{
	DEVTREE_CONST struct soc_intel_skylake_config *config;

	/* Look up the device in devicetree */
	DEVTREE_CONST struct device *dev = dev_find_slot(0, PCH_DEVFN_PMC);
	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}
	config = dev->chip_info;

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
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_PMC;
#else
	struct device *dev = PCH_DEV_PMC;
#endif
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
int soc_prev_sleep_state(const struct chipset_power_state *ps,
						int prev_sleep_state)
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
	uint16_t tcobase;
	uint8_t *pmc;

	tcobase = smbus_tco_regs();

	ps->tco1_sts = inw(tcobase + TCO1_STS);
	ps->tco2_sts = inw(tcobase + TCO2_STS);

	printk(BIOS_DEBUG, "TCO_STS:   %04x %04x\n",
	       ps->tco1_sts, ps->tco2_sts);

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

int acpi_get_sleep_type(void)
{
	struct chipset_power_state *ps;

	ps = pmc_get_power_state();
	return ps->prev_sleep_state;
}
