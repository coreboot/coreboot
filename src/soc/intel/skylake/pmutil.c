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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/*
 * Helper functions for dealing with power management registers
 * and the differences between PCH variants.
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <stdlib.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/smbus.h>

/* Print status bits with descriptive names */
static void print_status_bits(u32 status, const char *bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = 31; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(BIOS_DEBUG, "%s ", bit_names[i]);
			else
				printk(BIOS_DEBUG, "BIT%d ", i);
		}
	}
}

/* Print status bits as GPIO numbers */
static void print_gpio_status(u32 status, int start)
{
	int i;

	if (!status)
		return;

	for (i = 31; i >= 0; i--) {
		if (status & (1 << i))
			printk(BIOS_DEBUG, "GPIO%d ", start + i);
	}
}


/*
 * PM1_CNT
 */

/* Enable events in PM1 control register */
void enable_pm1_control(u32 mask)
{
	u32 pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	pm1_cnt |= mask;
	outl(pm1_cnt, ACPI_BASE_ADDRESS + PM1_CNT);
}

/* Disable events in PM1 control register */
void disable_pm1_control(u32 mask)
{
	u32 pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	pm1_cnt &= ~mask;
	outl(pm1_cnt, ACPI_BASE_ADDRESS + PM1_CNT);
}


/*
 * PM1
 */

/* Clear and return PM1 status register */
static u16 reset_pm1_status(void)
{
	u16 pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	outw(pm1_sts, ACPI_BASE_ADDRESS + PM1_STS);
	return pm1_sts;
}

/* Print PM1 status bits */
static u16 print_pm1_status(u16 pm1_sts)
{
	const char *pm1_sts_bits[] = {
		[0] = "TMROF",
		[4] = "BM",
		[5] = "GBL",
		[8] = "PWRBTN",
		[10] = "RTC",
		[11] = "PRBTNOR",
		[14] = "PCIEXPWAK",
		[15] = "WAK",
	};

	if (!pm1_sts)
		return 0;

	printk(BIOS_SPEW, "PM1_STS: ");
	print_status_bits(pm1_sts, pm1_sts_bits);
	printk(BIOS_SPEW, "\n");

	return pm1_sts;
}

/* Print, clear, and return PM1 status */
u16 clear_pm1_status(void)
{
	return print_pm1_status(reset_pm1_status());
}

/* Set the PM1 register to events */
void enable_pm1(u16 events)
{
	outw(events, ACPI_BASE_ADDRESS + PM1_EN);
}


/*
 * SMI
 */

/* Clear and return SMI status register */
static u32 reset_smi_status(void)
{
	u32 smi_sts = inl(ACPI_BASE_ADDRESS + SMI_STS);
	outl(smi_sts, ACPI_BASE_ADDRESS + SMI_STS);
	return smi_sts;
}

/* Print SMI status bits */
static u32 print_smi_status(u32 smi_sts)
{
	const char *smi_sts_bits[] = {
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
		[27] = "GPIO_UNLOCK"
	};

	if (!smi_sts)
		return 0;

	printk(BIOS_DEBUG, "SMI_STS: ");
	print_status_bits(smi_sts, smi_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return smi_sts;
}

/* Print, clear, and return SMI status */
u32 clear_smi_status(void)
{
	return print_smi_status(reset_smi_status());
}

/* Enable SMI event */
void enable_smi(u32 mask)
{
	u32 smi_en = inl(ACPI_BASE_ADDRESS + SMI_EN);
	smi_en |= mask;
	outl(smi_en, ACPI_BASE_ADDRESS + SMI_EN);
}

/* Disable SMI event */
void disable_smi(u32 mask)
{
	u32 smi_en = inl(ACPI_BASE_ADDRESS + SMI_EN);
	smi_en &= ~mask;
	outl(smi_en, ACPI_BASE_ADDRESS + SMI_EN);
}

/*
 * TCO
 */

/* Clear TCO status and return events that are enabled and active */
static u32 reset_tco_status(void)
{
	u16 tco1_sts;
	u16 tco2_sts;
	u16 tcobase;

	tcobase = pmc_tco_regs();

	/* TCO Status 2 register*/
	tco2_sts = inw(tcobase + TCO2_STS);
	tco2_sts |= (TCO2_STS_SECOND_TO | TCO2_STS_BOOT);
	outw(tco2_sts, tcobase + TCO2_STS);

	/* TCO Status 1 register*/
	tco1_sts = inw(tcobase + TCO1_STS);

	/* Clear SECOND_TO_STS bit */
	if (tco2_sts & TCO2_STS_SECOND_TO)
		outw(tco2_sts & ~TCO2_STS_SECOND_TO, tcobase + TCO2_STS);

	return (tco2_sts << 16) | tco1_sts;
}

/* Print TCO status bits */
static u32 print_tco_status(u32 tco_sts)
{
	const char *tco_sts_bits[] = {
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

	if (!tco_sts)
		return 0;

	printk(BIOS_DEBUG, "TCO_STS: ");
	print_status_bits(tco_sts, tco_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return tco_sts;
}

/* Print, clear, and return TCO status */
u32 clear_tco_status(void)
{
	return print_tco_status(reset_tco_status());
}

/* Enable TCO SCI */
void enable_tco_sci(void)
{
	/* Clear pending events */
	outl(TCOSCI_STS, ACPI_BASE_ADDRESS + GPE0_STS(3));

	/* Enable TCO SCI events */
	enable_gpe(TCOSCI_EN);
}


/*
 * GPE0
 */

/* Clear a GPE0 status and return events that are enabled and active */
static u32 reset_gpe(u16 sts_reg, u16 en_reg)
{
	u32 gpe0_sts = inl(ACPI_BASE_ADDRESS + sts_reg);
	u32 gpe0_en = inl(ACPI_BASE_ADDRESS + en_reg);

	outl(gpe0_sts, ACPI_BASE_ADDRESS + sts_reg);

	/* Only report enabled events */
	return gpe0_sts & gpe0_en;
}

/* Print GPE0 status bits */
static u32 print_gpe_status(u32 gpe0_sts, const char *bit_names[])
{
	if (!gpe0_sts)
		return 0;

	printk(BIOS_DEBUG, "GPE0_STS: ");
	print_status_bits(gpe0_sts, bit_names);
	printk(BIOS_DEBUG, "\n");

	return gpe0_sts;
}

/* Print GPE0 GPIO status bits */
static u32 print_gpe_gpio(u32 gpe0_sts, int start)
{
	if (!gpe0_sts)
		return 0;

	printk(BIOS_DEBUG, "GPE0_STS: ");
	print_gpio_status(gpe0_sts, start);
	printk(BIOS_DEBUG, "\n");

	return gpe0_sts;
}

/* Clear all GPE status and return "standard" GPE event status */
u32 clear_gpe_status(void)
{
	const char *gpe0_sts_3_bits[] = {
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

	print_gpe_gpio(reset_gpe(GPE0_STS(GPE_31_0), GPE0_EN(GPE_31_0)), 0);
	print_gpe_gpio(reset_gpe(GPE0_STS(GPE_63_32), GPE0_EN(GPE_63_32)), 32);
	print_gpe_gpio(reset_gpe(GPE0_STS(GPE_95_64), GPE0_EN(GPE_95_64)), 64);
	return print_gpe_status(reset_gpe(GPE0_STS(GPE_STD), GPE0_EN(GPE_STD)),
				gpe0_sts_3_bits);
}

/* Enable all requested GPE */
void enable_all_gpe(u32 set1, u32 set2, u32 set3, u32 set4)
{
	outl(set1, ACPI_BASE_ADDRESS + GPE0_EN(GPE_31_0));
	outl(set2, ACPI_BASE_ADDRESS + GPE0_EN(GPE_63_32));
	outl(set3, ACPI_BASE_ADDRESS + GPE0_EN(GPE_95_64));
	outl(set4, ACPI_BASE_ADDRESS + GPE0_EN(GPE_STD));
}

/* Disable all GPE */
void disable_all_gpe(void)
{
	enable_all_gpe(0, 0, 0, 0);
}

/* Enable a standard GPE */
void enable_gpe(u32 mask)
{
	u32 gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN(GPE_STD));
	gpe0_en |= mask;
	outl(gpe0_en, ACPI_BASE_ADDRESS + GPE0_EN(GPE_STD));
}

/* Disable a standard GPE */
void disable_gpe(u32 mask)
{
	u32 gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN(GPE_STD));
	gpe0_en &= ~mask;
	outl(gpe0_en, ACPI_BASE_ADDRESS + GPE0_EN(GPE_STD));
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

	return (void *)(uintptr_t)reg32;
}

uint16_t pmc_tco_regs(void)
{
	uint16_t reg16;

	reg16 = pci_read_config16(PCH_DEV_SMBUS, TCOBASE);

	reg16 &= ~0x1f;

	return reg16;
}
