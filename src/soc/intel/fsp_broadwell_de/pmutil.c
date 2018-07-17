/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Siemens AG
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

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>

/* Print status bits with descriptive names */
static void print_status_bits(u32 status, const char *const bit_names[])
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
	static const char *const pm1_sts_bits[] = {
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
