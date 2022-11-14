/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/common/gpio.h>

#include "pmutil.h"

void alt_gpi_mask(u16 clr, u16 set)
{
	u16 alt_gp = read_pmbase16(ALT_GP_SMI_EN);
	alt_gp &= ~clr;
	alt_gp |= set;
	write_pmbase16(ALT_GP_SMI_EN, alt_gp);
}

void gpe0_mask(u32 clr, u32 set)
{
	u32 gpe0 = read_pmbase32(GPE0_EN);
	gpe0 &= ~clr;
	gpe0 |= set;
	write_pmbase32(GPE0_EN, gpe0);
}

/**
 * @brief read and clear PM1_STS
 * @return PM1_STS register
 */
u16 reset_pm1_status(void)
{
	u16 reg16 = read_pmbase16(PM1_STS);
	/* set status bits are cleared by writing 1 to them */
	write_pmbase16(PM1_STS, reg16);

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

	int reg16 = read_pmbase16(PM1_EN);
	printk(BIOS_SPEW, "PM1_EN: %x\n", reg16);
}

/**
 * @brief read and clear SMI_STS
 * @return SMI_STS register
 */
u32 reset_smi_status(void)
{
	u32 reg32;

	reg32 = read_pmbase32(SMI_STS);
	/* set status bits are cleared by writing 1 to them */
	write_pmbase32(SMI_STS, reg32);

	return reg32;
}

void dump_smi_status(u32 smi_sts)
{
	printk(BIOS_DEBUG, "SMI_STS: ");
	if (smi_sts & (1 << 26)) printk(BIOS_DEBUG, "SPI ");
	if (smi_sts & (1 << 25)) printk(BIOS_DEBUG, "EL_SMI ");
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
	u32 reg_h = 0, reg_l;

	reg_l = read_pmbase32(GPE0_STS);
	if (GPE0_HAS_64_EVENTS)
		reg_h = read_pmbase32(GPE0_STS + 4);
	/* set status bits are cleared by writing 1 to them */
	write_pmbase32(GPE0_STS, reg_l);
	if (GPE0_HAS_64_EVENTS)
		write_pmbase32(GPE0_STS + 4, reg_h);

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
	if (gpe0_sts & (1 <<  5)) printk(BIOS_DEBUG, "AC97/USB5 ");
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
	u32 reg32;

	reg32 = read_pmbase32(TCO1_STS);
	/*
	 * set status bits are cleared by writing 1 to them, but don't
	 * clear BOOT_STS before SECOND_TO_STS.
	 */
	write_pmbase32(TCO1_STS, reg32 & ~BOOT_STS);
	if (reg32 & BOOT_STS)
		write_pmbase32(TCO1_STS, BOOT_STS);

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
	u16 reg16;

	reg16 = read_pmbase16(ALT_GP_SMI_STS);
	/* set status bits are cleared by writing 1 to them */
	write_pmbase16(ALT_GP_SMI_STS, reg16);

	return reg16;
}

void dump_all_status(void)
{
	dump_smi_status(reset_smi_status());
	dump_pm1_status(reset_pm1_status());
	dump_gpe0_status(reset_gpe0_status());
	dump_alt_gp_smi_status(reset_alt_gp_smi_status());
	dump_tco_status(reset_tco_status());
}
