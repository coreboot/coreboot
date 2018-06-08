/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <device/pci_def.h>
#include <cpu/x86/smm.h>
#include <elog.h>
#include <pc80/mc146818rtc.h>

#include <southbridge/intel/common/gpio.h>

#include "pmutil.h"

void alt_gpi_mask(u16 clr, u16 set)
{
	u16 pmbase = get_pmbase();
	u16 alt_gp = inw(pmbase + ALT_GP_SMI_EN);
	alt_gp &= ~clr;
	alt_gp |= set;
	outw(alt_gp, pmbase + ALT_GP_SMI_EN);
}

void gpe0_mask(u32 clr, u32 set)
{
	u16 pmbase = get_pmbase();
	u32 gpe0 = inl(pmbase + GPE0_EN);
	gpe0 &= ~clr;
	gpe0 |= set;
	outl(gpe0, pmbase + GPE0_EN);
}

/**
 * @brief read and clear PM1_STS
 * @return PM1_STS register
 */
u16 reset_pm1_status(void)
{
	u16 pmbase = get_pmbase();
	u16 reg16;

	reg16 = inw(pmbase + PM1_STS);
	/* set status bits are cleared by writing 1 to them */
	outw(reg16, pmbase + PM1_STS);

	return reg16;
}

void dump_pm1_status(u16 pm1_sts)
{
	printk(BIOS_SPEW, "PM1_STS: ");
	if (pm1_sts & (1 << 15)) printk(BIOS_SPEW, "WAK ");
	if (pm1_sts & (1 << 14)) printk(BIOS_SPEW, "PCIEXPWAK ");
	if (pm1_sts & (1 << 11)) printk(BIOS_SPEW, "PRBTNOR ");
	if (pm1_sts & (1 << 10)) printk(BIOS_SPEW, "RTC ");
	if (pm1_sts & (1 <<  8)) printk(BIOS_SPEW, "PWRBTN ");
	if (pm1_sts & (1 <<  5)) printk(BIOS_SPEW, "GBL ");
	if (pm1_sts & (1 <<  4)) printk(BIOS_SPEW, "BM ");
	if (pm1_sts & (1 <<  0)) printk(BIOS_SPEW, "TMROF ");
	printk(BIOS_SPEW, "\n");
	int reg16 = inw(get_pmbase() + PM1_EN);
	printk(BIOS_SPEW, "PM1_EN: %x\n", reg16);
}

/**
 * @brief read and clear SMI_STS
 * @return SMI_STS register
 */
u32 reset_smi_status(void)
{
	u16 pmbase = get_pmbase();
	u32 reg32;

	reg32 = inl(pmbase + SMI_STS);
	/* set status bits are cleared by writing 1 to them */
	outl(reg32, pmbase + SMI_STS);

	return reg32;
}

void dump_smi_status(u32 smi_sts)
{
	printk(BIOS_DEBUG, "SMI_STS: ");
	if (smi_sts & (1 << 26)) printk(BIOS_DEBUG, "SPI ");
	if (smi_sts & (1 << 21)) printk(BIOS_DEBUG, "MONITOR ");
	if (smi_sts & (1 << 20)) printk(BIOS_DEBUG, "PCI_EXP_SMI ");
	if (smi_sts & (1 << 18)) printk(BIOS_DEBUG, "INTEL_USB2 ");
	if (smi_sts & (1 << 17)) printk(BIOS_DEBUG, "LEGACY_USB2 ");
	if (smi_sts & (1 << 16)) printk(BIOS_DEBUG, "SMBUS_SMI ");
	if (smi_sts & (1 << 15)) printk(BIOS_DEBUG, "SERIRQ_SMI ");
	if (smi_sts & (1 << 14)) printk(BIOS_DEBUG, "PERIODIC ");
	if (smi_sts & (1 << 13)) printk(BIOS_DEBUG, "TCO ");
	if (smi_sts & (1 << 12)) printk(BIOS_DEBUG, "DEVMON ");
	if (smi_sts & (1 << 11)) printk(BIOS_DEBUG, "MCSMI ");
	if (smi_sts & (1 << 10)) printk(BIOS_DEBUG, "GPI ");
	if (smi_sts & (1 <<  9)) printk(BIOS_DEBUG, "GPE0 ");
	if (smi_sts & (1 <<  8)) printk(BIOS_DEBUG, "PM1 ");
	if (smi_sts & (1 <<  6)) printk(BIOS_DEBUG, "SWSMI_TMR ");
	if (smi_sts & (1 <<  5)) printk(BIOS_DEBUG, "APM ");
	if (smi_sts & (1 <<  4)) printk(BIOS_DEBUG, "SLP_SMI ");
	if (smi_sts & (1 <<  3)) printk(BIOS_DEBUG, "LEGACY_USB ");
	if (smi_sts & (1 <<  2)) printk(BIOS_DEBUG, "BIOS ");
	printk(BIOS_DEBUG, "\n");
}


/**
 * @brief read and clear GPE0_STS
 * @return GPE0_STS register
 */
u64 reset_gpe0_status(void)
{
	u16 pmbase = get_pmbase();
	u32 reg_h, reg_l;

	reg_l = inl(pmbase + GPE0_STS);
	reg_h = inl(pmbase + GPE0_STS + 4);
	/* set status bits are cleared by writing 1 to them */
	outl(reg_l, pmbase + GPE0_STS);
	outl(reg_h, pmbase + GPE0_STS + 4);

	return (((u64)reg_h) << 32) | reg_l;
}

void dump_gpe0_status(u64 gpe0_sts)
{
	int i;
	printk(BIOS_DEBUG, "GPE0_STS: ");
	if (gpe0_sts & (1LL << 32)) printk(BIOS_DEBUG, "USB6 ");
	for (i=31; i>= 16; i--) {
		if (gpe0_sts & (1 << i)) printk(BIOS_DEBUG, "GPIO%d ", (i-16));
	}
	if (gpe0_sts & (1 << 14)) printk(BIOS_DEBUG, "USB4 ");
	if (gpe0_sts & (1 << 13)) printk(BIOS_DEBUG, "PME_B0 ");
	if (gpe0_sts & (1 << 12)) printk(BIOS_DEBUG, "USB3 ");
	if (gpe0_sts & (1 << 11)) printk(BIOS_DEBUG, "PME ");
	if (gpe0_sts & (1 << 10)) printk(BIOS_DEBUG, "EL_SCI/BATLOW ");
	if (gpe0_sts & (1 <<  9)) printk(BIOS_DEBUG, "PCI_EXP ");
	if (gpe0_sts & (1 <<  8)) printk(BIOS_DEBUG, "RI ");
	if (gpe0_sts & (1 <<  7)) printk(BIOS_DEBUG, "SMB_WAK ");
	if (gpe0_sts & (1 <<  6)) printk(BIOS_DEBUG, "TCO_SCI ");
	if (gpe0_sts & (1 <<  5)) printk(BIOS_DEBUG, "USB5 ");
	if (gpe0_sts & (1 <<  4)) printk(BIOS_DEBUG, "USB2 ");
	if (gpe0_sts & (1 <<  3)) printk(BIOS_DEBUG, "USB1 ");
	if (gpe0_sts & (1 <<  2)) printk(BIOS_DEBUG, "SWGPE ");
	if (gpe0_sts & (1 <<  1)) printk(BIOS_DEBUG, "HOT_PLUG ");
	if (gpe0_sts & (1 <<  0)) printk(BIOS_DEBUG, "THRM ");
	printk(BIOS_DEBUG, "\n");
}

/**
 * @brief read and clear TCOx_STS
 * @return TCOx_STS registers
 */
u32 reset_tco_status(void)
{
	u32 tcobase = get_pmbase() + 0x60;
	u32 reg32;

	reg32 = inl(tcobase + 0x04);
	/* set status bits are cleared by writing 1 to them */
	outl(reg32 & ~(1<<18), tcobase + 0x04); //  Don't clear BOOT_STS before SECOND_TO_STS
	if (reg32 & (1 << 18))
		outl(reg32 & (1<<18), tcobase + 0x04); // clear BOOT_STS

	return reg32;
}


void dump_tco_status(u32 tco_sts)
{
	printk(BIOS_DEBUG, "TCO_STS: ");
	if (tco_sts & (1 << 20)) printk(BIOS_DEBUG, "SMLINK_SLV ");
	if (tco_sts & (1 << 18)) printk(BIOS_DEBUG, "BOOT ");
	if (tco_sts & (1 << 17)) printk(BIOS_DEBUG, "SECOND_TO ");
	if (tco_sts & (1 << 16)) printk(BIOS_DEBUG, "INTRD_DET ");
	if (tco_sts & (1 << 12)) printk(BIOS_DEBUG, "DMISERR ");
	if (tco_sts & (1 << 10)) printk(BIOS_DEBUG, "DMISMI ");
	if (tco_sts & (1 <<  9)) printk(BIOS_DEBUG, "DMISCI ");
	if (tco_sts & (1 <<  8)) printk(BIOS_DEBUG, "BIOSWR ");
	if (tco_sts & (1 <<  7)) printk(BIOS_DEBUG, "NEWCENTURY ");
	if (tco_sts & (1 <<  3)) printk(BIOS_DEBUG, "TIMEOUT ");
	if (tco_sts & (1 <<  2)) printk(BIOS_DEBUG, "TCO_INT ");
	if (tco_sts & (1 <<  1)) printk(BIOS_DEBUG, "SW_TCO ");
	if (tco_sts & (1 <<  0)) printk(BIOS_DEBUG, "NMI2SMI ");
	printk(BIOS_DEBUG, "\n");
}

/**
 * @brief Set the EOS bit
 */
void smi_set_eos(void)
{
	u16 pmbase = get_pmbase();
	u8 reg8;

	reg8 = inb(pmbase + SMI_EN);
	reg8 |= EOS;
	outb(reg8, pmbase + SMI_EN);
}


void dump_alt_gp_smi_status(u16 alt_gp_smi_sts)
{
	int i;
	printk(BIOS_DEBUG, "ALT_GP_SMI_STS: ");
	for (i=15; i>= 0; i--) {
		if (alt_gp_smi_sts & (1 << i)) printk(BIOS_DEBUG, "GPI%d ", i);
	}
	printk(BIOS_DEBUG, "\n");
}

/**
 * @brief read and clear ALT_GP_SMI_STS
 * @return ALT_GP_SMI_STS register
 */
u16 reset_alt_gp_smi_status(void)
{
	u16 pmbase = get_pmbase();
	u16 reg16;

	reg16 = inl(pmbase + ALT_GP_SMI_STS);
	/* set status bits are cleared by writing 1 to them */
	outl(reg16, pmbase + ALT_GP_SMI_STS);

	return reg16;
}
