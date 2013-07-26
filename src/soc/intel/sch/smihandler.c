/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include "sch.h"

#define DEBUG_SMI

/* I945 */
#define SMRAM		0x9d
#define   D_OPEN	(1 << 6)
#define   D_CLS		(1 << 5)
#define   D_LCK		(1 << 4)
#define   G_SMRANE	(1 << 3)
#define   C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

/* ICH7 */
#define PM1_STS		0x00
#define PM1_EN		0x02
#define PM1_CNT		0x04
#define PM1_TMR		0x08
#define PROC_CNT	0x10
#define LV2		0x14
#define LV3		0x15
#define LV4		0x16
#define PM2_CNT		0x20 // mobile only
#define GPE0_STS	0x28
#define GPE0_EN		0x2c
#define SMI_EN		0x30
#define   EL_SMI_EN	 (1 << 25) // Intel Quick Resume Technology
#define   INTEL_USB2_EN	 (1 << 18) // Intel-Specific USB2 SMI logic
#define   LEGACY_USB2_EN (1 << 17) // Legacy USB2 SMI logic
#define   PERIODIC_EN	 (1 << 14) // SMI on PERIODIC_STS in SMI_STS
#define   TCO_EN	 (1 << 13) // Enable TCO Logic (BIOSWE et al)
#define   MCSMI_EN	 (1 << 11) // Trap microcontroller range access
#define   BIOS_RLS	 (1 <<  7) // asserts SCI on bit set
#define   SWSMI_TMR_EN	 (1 <<  6) // start software smi timer on bit set
#define   APMC_EN	 (1 <<  5) // Writes to APM_CNT cause SMI#
#define   SLP_SMI_EN	 (1 <<  4) // Write to SLP_EN in PM1_CNT asserts SMI#
#define   LEGACY_USB_EN  (1 <<  3) // Legacy USB circuit SMI logic
#define   BIOS_EN	 (1 <<  2) // Assert SMI# on setting GBL_RLS bit
#define   EOS		 (1 <<  1) // End of SMI (deassert SMI#)
#define   GBL_SMI_EN	 (1 <<  0) // SMI# generation at all?
#define SMI_STS		0x34
#define ALT_GP_SMI_EN	0x38
#define ALT_GP_SMI_STS	0x3a
#define GPE_CNTL	0x42
#define DEVACT_STS	0x44
#define SS_CNT		0x50
#define C3_RES		0x54

//#include "i82801gx_nvs.h"

/* While we read PMBASE dynamically in case it changed, let's
 * initialize it with a sane value
 */
static u16 pmbase = DEFAULT_PMBASE;

// disabled because SMM doesn't actually work yet
#if 0
/**
 * @brief read and clear PM1_STS
 * @return PM1_STS register
 */
static u16 reset_pm1_status(void)
{
	u16 reg16;

	reg16 = inw(pmbase + PM1_STS);
	/* set status bits are cleared by writing 1 to them */
	outw(reg16, pmbase + PM1_STS);

	return reg16;
}

static void dump_pm1_status(u16 pm1_sts)
{
	printk(BIOS_DEBUG, "PM1_STS: ");
	if (pm1_sts & (1 << 15)) printk(BIOS_DEBUG, "WAK ");
	if (pm1_sts & (1 << 14)) printk(BIOS_DEBUG, "PCIEXPWAK ");
	if (pm1_sts & (1 << 11)) printk(BIOS_DEBUG, "PRBTNOR ");
	if (pm1_sts & (1 << 10)) printk(BIOS_DEBUG, "RTC ");
	if (pm1_sts & (1 <<  8)) printk(BIOS_DEBUG, "PWRBTN ");
	if (pm1_sts & (1 <<  5)) printk(BIOS_DEBUG, "GBL ");
	if (pm1_sts & (1 <<  4)) printk(BIOS_DEBUG, "BM ");
	if (pm1_sts & (1 <<  0)) printk(BIOS_DEBUG, "TMROF ");
	printk(BIOS_DEBUG, "\n");
}

/**
 * @brief read and clear SMI_STS
 * @return SMI_STS register
 */
static u32 reset_smi_status(void)
{
	u32 reg32;

	reg32 = inl(pmbase + SMI_STS);
	/* set status bits are cleared by writing 1 to them */
	outl(reg32, pmbase + SMI_STS);

	return reg32;
}

static void dump_smi_status(u32 smi_sts)
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
static u32 reset_gpe0_status(void)
{
	u32 reg32;

	reg32 = inl(pmbase + GPE0_STS);
	/* set status bits are cleared by writing 1 to them */
	outl(reg32, pmbase + GPE0_STS);

	return reg32;
}

static void dump_gpe0_status(u32 gpe0_sts)
{
	int i;
	printk(BIOS_DEBUG, "GPE0_STS: ");
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
	if (gpe0_sts & (1 <<  5)) printk(BIOS_DEBUG, "AC97 ");
	if (gpe0_sts & (1 <<  4)) printk(BIOS_DEBUG, "USB2 ");
	if (gpe0_sts & (1 <<  3)) printk(BIOS_DEBUG, "USB1 ");
	if (gpe0_sts & (1 <<  2)) printk(BIOS_DEBUG, "HOT_PLUG ");
	if (gpe0_sts & (1 <<  0)) printk(BIOS_DEBUG, "THRM ");
	printk(BIOS_DEBUG, "\n");
}


/**
 * @brief read and clear TCOx_STS
 * @return TCOx_STS registers
 */
static u32 reset_tco_status(void)
{
	u32 tcobase = pmbase + 0x60;
	u32 reg32;

	reg32 = inl(tcobase + 0x04);
	/* set status bits are cleared by writing 1 to them */
	outl(reg32 & ~(1<<18), tcobase + 0x04); //  Don't clear BOOT_STS before SECOND_TO_STS
	if (reg32 & (1 << 18))
		outl(reg32 & (1<<18), tcobase + 0x04); // clear BOOT_STS

	return reg32;
}


static void dump_tco_status(u32 tco_sts)
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
#endif

int southbridge_io_trap_handler(int smif)
{
	//global_nvs_t *gnvs = (global_nvs_t *)0xc00;

	switch (smif) {
	case 0x32:
		printk(BIOS_DEBUG, "OS Init\n");
		//gnvs->smif = 0;
		break;
	default:
		/* Not handled */
		return 0;
	}

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	//gnvs->smif = 0;
	return 1; /* IO trap handled */
}

/**
 * @brief Set the EOS bit
 */
void southbridge_smi_set_eos(void)
{
	u8 reg8;

	reg8 = inb(pmbase + SMI_EN);
	reg8 |= EOS;
	outb(reg8, pmbase + SMI_EN);
}

/**
 * @brief Interrupt handler for SMI#
 * @param node
 * @param state_save
 */
void southbridge_smi_handler(unsigned int node, smm_state_save_area_t *state_save)
{
// FIXME: the necessary magic isn't available yet. the code
// below is a partially adapted ICH7 version of the handler
#if 0
	u8 reg8;
	u16 pmctrl;
	u16 pm1_sts;
	u32 smi_sts, gpe0_sts, tco_sts;

	pmbase = pci_read_config16(PCI_DEV(0, 0x1f, 0), 0x48) & 0xfffc;
	printk(BIOS_SPEW, "SMI#: pmbase = 0x%04x\n", pmbase);

	/* We need to clear the SMI status registers, or we won't see what's
	 * happening in the following calls.
	 */
	smi_sts = reset_smi_status();
	dump_smi_status(smi_sts);

	if (smi_sts & (1 << 21)) { // MONITOR
		global_nvs_t *gnvs = (global_nvs_t *)0xc00;
		u32 reg32;

		reg32 = RCBA32(0x1e00);  TRSR - Trap Status Register
//#if 0
		/* Comment in for some useful debug */
		for (i=0; i<4; i++) {
			if (reg32 & (1 << i)) {
				printk(BIOS_DEBUG, "  io trap #%d\n", i);
			}
		}
//#endif
		RCBA32(0x1e00) = reg32;  TRSR

		reg32 = RCBA32(0x1e10);

		if ((reg32 & 0xfffc) != 0x808) {
			printk(BIOS_DEBUG, "  trapped io address = 0x%x\n", reg32 & 0xfffc);
			printk(BIOS_DEBUG, "  AHBE = %x\n", (reg32 >> 16) & 0xf);
			printk(BIOS_DEBUG, "  read/write: %s\n", (reg32 & (1 << 24)) ? "read" :
				"write");
		}

		if (!(reg32 & (1 << 24))) {
			/* Write Cycle */
			reg32 = RCBA32(0x1e18);
			printk(BIOS_DEBUG, "  iotrap written data = 0x%08x\n", reg32);

		}

		if (gnvs->smif)
			io_trap_handler(gnvs->smif); // call function smif
	}

	if (smi_sts & (1 << 13)) { // TCO
		tco_sts = reset_tco_status();
		dump_tco_status(tco_sts);

		if (tco_sts & (1 << 8)) { // BIOSWR
			u8 bios_cntl;
			bios_cntl = pci_read_config16(PCI_DEV(0, 0x1f, 0), 0xdc);
			if (bios_cntl & 1) {
				/* BWE is RW, so the SMI was caused by a
				 * write to BWE, not by a write to the BIOS
				 */

				/* This is the place where we notice someone
				 * is trying to tinker with the BIOS. We are
				 * trying to be nice and just ignore it. A more
				 * resolute answer would be to power down the
				 * box.
				 */
				printk(BIOS_DEBUG, "Switching back to RO\n");
				pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xdc, (bios_cntl & ~1));
			} /* No else for now? */
		}
	}

	if (smi_sts & (1 << 8)) { // PM1
		pm1_sts = reset_pm1_status();
		dump_pm1_status(pm1_sts);
	}

	if (smi_sts & (1 << 9)) { // GPE0
		gpe0_sts = reset_gpe0_status();
		dump_gpe0_status(gpe0_sts);
	}

	if (smi_sts & (1 << 5)) { // APM
		/* Emulate B2 register as the FADT / Linux expects it */

		reg8 = inb(0xb2);
		switch (reg8) {
		case ACPI_DISABLE:
			pmctrl = inw(pmbase + 0x04);
			pmctrl |= (1 << 0);
			outw(pmctrl, pmbase + 0x04);
			printk(BIOS_DEBUG, "SMI#: ACPI disabled.\n");
			break;
		case ACPI_ENABLE:
			pmctrl = inw(pmbase + 0x04);
			pmctrl &= ~(1 << 0);
			outw(pmctrl, pmbase + 0x04);
			printk(BIOS_DEBUG, "SMI#: ACPI enabled.\n");
			break;
		}
	}

	if (smi_sts & (1 << 4)) { // SLP_SMI
		u32 reg32;

		/* First, disable further SMIs */
		reg8 = inb(pmbase + SMI_EN);
		reg8 &= ~SLP_SMI_EN;
		outb(reg8, pmbase + SMI_EN);

		/* Next, do the deed, we should change
		 * power on after power loss bits here
		 * if we're going to S5
		 */

		/* Write back to the SLP register to cause the
		 * originally intended event again. We need to set BIT13
		 * (SLP_EN) though to make the sleep happen.
		 */
		reg32 = inl(pmbase + 0x04);
		printk(BIOS_DEBUG, "SMI#: SLP = 0x%08x\n", reg32);
		printk(BIOS_DEBUG, "SMI#: Powering off.\n");
		outl(reg32 | (1 << 13), pmbase + 0x04);
	}
#endif
}
