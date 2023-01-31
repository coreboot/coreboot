/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Helper functions for dealing with power management registers
 * and the differences between LynxPoint-H and LynxPoint-LP.
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_common.h>
#include <southbridge/intel/common/rtc.h>
#include "pch.h"

#if CONFIG(INTEL_LYNXPOINT_LP)
#include "lp_gpio.h"
#endif

/* These defines are here to handle the LP variant code dynamically. If these
 * values are defined in lp_gpio.h but when a non-LP board is being built, the
 * build will fail. */
#define GPIO_ALT_GPI_SMI_STS	0x50
#define GPIO_ALT_GPI_SMI_EN	0x54

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
	u32 pm1_cnt = inl(get_pmbase() + PM1_CNT);
	pm1_cnt |= mask;
	outl(pm1_cnt, get_pmbase() + PM1_CNT);
}

/* Disable events in PM1 control register */
void disable_pm1_control(u32 mask)
{
	u32 pm1_cnt = inl(get_pmbase() + PM1_CNT);
	pm1_cnt &= ~mask;
	outl(pm1_cnt, get_pmbase() + PM1_CNT);
}

/*
 * PM1
 */

/* Clear and return PM1 status register */
static u16 reset_pm1_status(void)
{
	u16 pm1_sts = inw(get_pmbase() + PM1_STS);
	outw(pm1_sts, get_pmbase() + PM1_STS);
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
	outw(events, get_pmbase() + PM1_EN);
}

/*
 * SMI
 */

/* Clear and return SMI status register */
static u32 reset_smi_status(void)
{
	u32 smi_sts = inl(get_pmbase() + SMI_STS);
	outl(smi_sts, get_pmbase() + SMI_STS);
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
	u32 smi_en = inl(get_pmbase() + SMI_EN);
	smi_en |= mask;
	outl(smi_en, get_pmbase() + SMI_EN);
}

/* Disable SMI event */
void disable_smi(u32 mask)
{
	u32 smi_en = inl(get_pmbase() + SMI_EN);
	smi_en &= ~mask;
	outl(smi_en, get_pmbase() + SMI_EN);
}

/*
 * ALT_GP_SMI
 */

/* Clear GPIO SMI status and return events that are enabled and active */
static u32 reset_alt_smi_status(void)
{
	u32 alt_sts, alt_en;

	if (pch_is_lp()) {
		/* LynxPoint-LP moves this to GPIO region as dword */
		alt_sts = inl(get_gpiobase() + GPIO_ALT_GPI_SMI_STS);
		outl(alt_sts, get_gpiobase() + GPIO_ALT_GPI_SMI_STS);

		alt_en = inl(get_gpiobase() + GPIO_ALT_GPI_SMI_EN);
	} else {
		u16 pmbase = get_pmbase();

		/* LynxPoint-H adds a second enable/status word */
		alt_sts = inw(pmbase + ALT_GP_SMI_STS2);
		outw(alt_sts & 0xffff, pmbase + ALT_GP_SMI_STS2);

		alt_sts <<= 16;
		alt_sts |= inw(pmbase + ALT_GP_SMI_STS);
		outw(alt_sts & 0xffff, pmbase + ALT_GP_SMI_STS);

		alt_en = inw(pmbase + ALT_GP_SMI_EN2);
		alt_en <<= 16;
		alt_en |= inw(pmbase + ALT_GP_SMI_EN);
	}

	/* Only report enabled events */
	return alt_sts & alt_en;
}

/* Print GPIO SMI status bits */
static u32 print_alt_smi_status(u32 alt_sts)
{
	if (!alt_sts)
		return 0;

	printk(BIOS_DEBUG, "ALT_STS: ");

	if (pch_is_lp()) {
		/* First 16 events are GPIO 32-47 */
		print_gpio_status(alt_sts & 0xffff, 32);
	} else {
		const char *alt_sts_bits_high[] = {
			[0] = "GPIO17",
			[1] = "GPIO19",
			[2] = "GPIO21",
			[3] = "GPIO22",
			[4] = "GPIO43",
			[5] = "GPIO56",
			[6] = "GPIO57",
			[7] = "GPIO60",
		};

		/* First 16 events are GPIO 0-15 */
		print_gpio_status(alt_sts & 0xffff, 0);
		print_status_bits(alt_sts >> 16, alt_sts_bits_high);
	}

	printk(BIOS_DEBUG, "\n");

	return alt_sts;
}

/* Print, clear, and return GPIO SMI status */
u32 clear_alt_smi_status(void)
{
	return print_alt_smi_status(reset_alt_smi_status());
}

/* Enable GPIO SMI events */
void enable_alt_smi(u32 mask)
{
	if (pch_is_lp()) {
		u32 alt_en;

		alt_en = inl(get_gpiobase() + GPIO_ALT_GPI_SMI_EN);
		alt_en |= mask;
		outl(alt_en, get_gpiobase() + GPIO_ALT_GPI_SMI_EN);
	} else {
		u16 pmbase = get_pmbase();
		u16 alt_en;

		/* Lower enable register */
		alt_en = inw(pmbase + ALT_GP_SMI_EN);
		alt_en |= mask & 0xffff;
		outw(alt_en, pmbase + ALT_GP_SMI_EN);

		/* Upper enable register */
		alt_en = inw(pmbase + ALT_GP_SMI_EN2);
		alt_en |= (mask >> 16) & 0xffff;
		outw(alt_en, pmbase + ALT_GP_SMI_EN2);
	}
}

/*
 * TCO
 */

/* Clear TCO status and return events that are active */
static u32 reset_tco_status(void)
{
	u32 tcobase = get_pmbase() + 0x60;
	u32 tco_sts = inl(tcobase + 0x04);

	/* Don't clear BOOT_STS before SECOND_TO_STS */
	outl(tco_sts & ~(1 << 18), tcobase + 0x04);

	/* Clear BOOT_STS */
	if (tco_sts & (1 << 18))
		outl(tco_sts & (1 << 18), tcobase + 0x04);

	return tco_sts;
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
	u16 gpe0_sts = pch_is_lp() ? LP_GPE0_STS_4 : GPE0_STS;

	/* Clear pending events */
	outl(TCOSCI_STS, get_pmbase() + gpe0_sts);

	/* Enable TCO SCI events */
	enable_gpe(TCOSCI_EN);
}

/*
 * GPE0
 */

/* Clear a GPE0 status and return events that are enabled and active */
static u32 reset_gpe_status(u16 sts_reg, u16 en_reg)
{
	u32 gpe0_sts = inl(get_pmbase() + sts_reg);
	u32 gpe0_en = inl(get_pmbase() + en_reg);

	outl(gpe0_sts, get_pmbase() + sts_reg);

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

/* Print, clear, and return LynxPoint-H GPE0 status */
static u32 clear_lpt_gpe_status(void)
{
	const char *gpe0_sts_bits_low[] = {
		[1] = "HOTPLUG",
		[2] = "SWGPE",
		[6] = "TCO_SCI",
		[7] = "SMB_WAK",
		[8] = "RI",
		[9] = "PCI_EXP",
		[10] = "BATLOW",
		[11] = "PME",
		[13] = "PME_B0",
		[16] = "GPIO0",
		[17] = "GPIO1",
		[18] = "GPIO2",
		[19] = "GPIO3",
		[20] = "GPIO4",
		[21] = "GPIO5",
		[22] = "GPIO6",
		[23] = "GPIO7",
		[24] = "GPIO8",
		[25] = "GPIO9",
		[26] = "GPIO10",
		[27] = "GPIO11",
		[28] = "GPIO12",
		[29] = "GPIO13",
		[30] = "GPIO14",
		[31] = "GPIO15",
	};
	const char *gpe0_sts_bits_high[] = {
		[3] = "GPIO27",
		[6] = "WADT",
		[24] = "GPIO17",
		[25] = "GPIO19",
		[26] = "GPIO21",
		[27] = "GPIO22",
		[28] = "GPIO43",
		[29] = "GPIO56",
		[30] = "GPIO57",
		[31] = "GPIO60",
	};

	/* High bits */
	print_gpe_status(reset_gpe_status(GPE0_STS_2, GPE0_EN_2),
			 gpe0_sts_bits_high);

	/* Standard GPE and GPIO 0-31 */
	return print_gpe_status(reset_gpe_status(GPE0_STS, GPE0_EN),
				gpe0_sts_bits_low);
}

/* Print, clear, and return LynxPoint-LP GPE0 status */
static u32 clear_lpt_lp_gpe_status(void)
{
	const char *gpe0_sts_4_bits[] = {
		[1] = "HOTPLUG",
		[2] = "SWGPE",
		[6] = "TCO_SCI",
		[7] = "SMB_WAK",
		[9] = "PCI_EXP",
		[10] = "BATLOW",
		[11] = "PME",
		[12] = "ME",
		[13] = "PME_B0",
		[16] = "GPIO27",
		[18] = "WADT"
	};

	/* GPIO 0-31 */
	print_gpe_gpio(reset_gpe_status(LP_GPE0_STS_1, LP_GPE0_EN_1), 0);

	/* GPIO 32-63 */
	print_gpe_gpio(reset_gpe_status(LP_GPE0_STS_2, LP_GPE0_EN_2), 32);

	/* GPIO 64-94 */
	print_gpe_gpio(reset_gpe_status(LP_GPE0_STS_3, LP_GPE0_EN_3), 64);

	/* Standard GPE */
	return print_gpe_status(reset_gpe_status(LP_GPE0_STS_4, LP_GPE0_EN_4),
				gpe0_sts_4_bits);
}

/* Clear all GPE status and return "standard" GPE event status */
u32 clear_gpe_status(void)
{
	if (pch_is_lp())
		return clear_lpt_lp_gpe_status();
	else
		return clear_lpt_gpe_status();
}

/* Enable all requested GPE */
void enable_all_gpe(u32 set1, u32 set2, u32 set3, u32 set4)
{
	u16 pmbase = get_pmbase();

	if (pch_is_lp()) {
		outl(set1, pmbase + LP_GPE0_EN_1);
		outl(set2, pmbase + LP_GPE0_EN_2);
		outl(set3, pmbase + LP_GPE0_EN_3);
		outl(set4, pmbase + LP_GPE0_EN_4);
	} else {
		outl(set1, pmbase + GPE0_EN);
		outl(set2, pmbase + GPE0_EN_2);
	}
}

/* Disable all GPE */
void disable_all_gpe(void)
{
	enable_all_gpe(0, 0, 0, 0);
}

/* Enable a standard GPE */
void enable_gpe(u32 mask)
{
	u32 gpe0_reg = pch_is_lp() ? LP_GPE0_EN_4 : GPE0_EN;
	u32 gpe0_en = inl(get_pmbase() + gpe0_reg);
	gpe0_en |= mask;
	outl(gpe0_en, get_pmbase() + gpe0_reg);
}

/* Disable a standard GPE */
void disable_gpe(u32 mask)
{
	u32 gpe0_reg = pch_is_lp() ? LP_GPE0_EN_4 : GPE0_EN;
	u32 gpe0_en = inl(get_pmbase() + gpe0_reg);
	gpe0_en &= ~mask;
	outl(gpe0_en, get_pmbase() + gpe0_reg);
}
