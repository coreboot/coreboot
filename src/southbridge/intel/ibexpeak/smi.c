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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#define __SIMPLE_DEVICE__

#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <arch/io.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <string.h>
#include "pch.h"

#if CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE || CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE
#include "northbridge/intel/sandybridge/sandybridge.h"
#endif

#if CONFIG_NORTHBRIDGE_INTEL_NEHALEM
#include "northbridge/intel/nehalem/nehalem.h"
#endif

extern unsigned char _binary_smm_start;
extern unsigned char _binary_smm_end;

/* While we read PMBASE dynamically in case it changed, let's
 * initialize it with a sane value
 */
static u16 pmbase = DEFAULT_PMBASE;

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
 * @brief read and clear ALT_GP_SMI_STS
 * @return ALT_GP_SMI_STS register
 */
static u16 reset_alt_gp_smi_status(void)
{
	u16 reg16;

	reg16 = inl(pmbase + ALT_GP_SMI_STS);
	/* set status bits are cleared by writing 1 to them */
	outl(reg16, pmbase + ALT_GP_SMI_STS);

	return reg16;
}

static void dump_alt_gp_smi_status(u16 alt_gp_smi_sts)
{
	int i;
	printk(BIOS_DEBUG, "ALT_GP_SMI_STS: ");
	for (i=15; i>= 0; i--) {
		if (alt_gp_smi_sts & (1 << i)) printk(BIOS_DEBUG, "GPI%d ", i);
	}
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



/**
 * @brief Set the EOS bit
 */
static void smi_set_eos(void)
{
	u8 reg8;

	reg8 = inb(pmbase + SMI_EN);
	reg8 |= EOS;
	outb(reg8, pmbase + SMI_EN);
}

extern uint8_t smm_relocation_start, smm_relocation_end;

static void smm_relocate(void)
{
	u32 smi_en;
	u16 pm1_en;
	u32 gpe0_en;

	printk(BIOS_DEBUG, "Initializing SMM handler...");

	pmbase = pci_read_config32(PCI_DEV(0, 0x1f, 0),
				   PMBASE) & 0xff80;

	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", pmbase);

	smi_en = inl(pmbase + SMI_EN);
	if (smi_en & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	/* copy the SMM relocation code */
	memcpy((void *)0x38000, &smm_relocation_start,
			&smm_relocation_end - &smm_relocation_start);

	printk(BIOS_DEBUG, "\n");
	dump_smi_status(reset_smi_status());
	dump_pm1_status(reset_pm1_status());
	dump_gpe0_status(reset_gpe0_status());
	dump_alt_gp_smi_status(reset_alt_gp_smi_status());
	dump_tco_status(reset_tco_status());

	/* Disable GPE0 PME_B0 */
	gpe0_en = inl(pmbase + GPE0_EN);
	gpe0_en &= ~PME_B0_EN;
	outl(gpe0_en, pmbase + GPE0_EN);

	pm1_en = 0;
	pm1_en |= PWRBTN_EN;
	pm1_en |= GBL_EN;
	outw(pm1_en, pmbase + PM1_EN);

	/* Enable SMI generation:
	 *  - on TCO events
	 *  - on APMC writes (io 0xb2)
	 *  - on writes to SLP_EN (sleep states)
	 *  - on writes to GBL_RLS (bios commands)
	 * No SMIs:
	 *  - on microcontroller writes (io 0x62/0x66)
	 */

	smi_en = 0; /* reset SMI enables */

#if 0
	smi_en |= LEGACY_USB2_EN | LEGACY_USB_EN;
#endif
	smi_en |= TCO_EN;
	smi_en |= APMC_EN;
#if DEBUG_PERIODIC_SMIS
	/* Set DEBUG_PERIODIC_SMIS in pch.h to debug using
	 * periodic SMIs.
	 */
	smi_en |= PERIODIC_EN;
#endif
	smi_en |= SLP_SMI_EN;
#if 0
	smi_en |= BIOS_EN;
#endif

	/* The following need to be on for SMIs to happen */
	smi_en |= EOS | GBL_SMI_EN;

	outl(smi_en, pmbase + SMI_EN);

	/**
	 * There are several methods of raising a controlled SMI# via
	 * software, among them:
	 *  - Writes to io 0xb2 (APMC)
	 *  - Writes to the Local Apic ICR with Delivery mode SMI.
	 *
	 * Using the local apic is a bit more tricky. According to
	 * AMD Family 11 Processor BKDG no destination shorthand must be
	 * used.
	 * The whole SMM initialization is quite a bit hardware specific, so
	 * I'm not too worried about the better of the methods at the moment
	 */

	/* raise an SMI interrupt */
	printk(BIOS_SPEW, "  ... raise SMI#\n");
	outb(0x00, 0xb2);
}

static int smm_handler_copied = 0;

static void smm_install(void)
{
	device_t dev = PCI_DEV(0, 0, 0);
	device_t qpdev = PCI_DEV(QUICKPATH_BUS, 0, 1);
	u32 smm_base = 0xa0000;
	struct ied_header ied = {
		.signature = "INTEL RSVD",
		.size = IED_SIZE,
		.reserved = {0},
	};

	/* The first CPU running this gets to copy the SMM handler. But not all
	 * of them.
	 */
	if (smm_handler_copied)
		return;
	smm_handler_copied = 1;

	/* enable the SMM memory window */
	pci_write_config8(qpdev, QPD0F1_SMRAM, D_OPEN | G_SMRAME | C_BASE_SEG);

#if CONFIG_SMM_TSEG
	smm_base = pci_read_config32(dev, TSEG) & ~1;
#endif

	/* copy the real SMM handler */
	printk(BIOS_DEBUG, "Installing SMM handler to 0x%08x\n", smm_base);
	memcpy((void *)smm_base, &_binary_smm_start,
	       (size_t)(&_binary_smm_end - &_binary_smm_start));

	/* copy the IED header into place */
	if (CONFIG_SMM_TSEG_SIZE > IED_SIZE) {
		/* Top of TSEG region */
		smm_base += CONFIG_SMM_TSEG_SIZE - IED_SIZE;
		printk(BIOS_DEBUG, "Installing IED header to 0x%08x\n",
		       smm_base);
		memcpy((void *)smm_base, &ied, sizeof(ied));
	}
	wbinvd();

	/* close the SMM memory window and enable normal SMM */
	pci_write_config8(qpdev, QPD0F1_SMRAM, G_SMRAME | C_BASE_SEG);
}

void smm_init(void)
{
#if CONFIG_ELOG
	/* Log events from chipset before clearing */
	pch_log_state();
#endif

	/* Put SMM code to 0xa0000 */
	smm_install();

	/* Put relocation code to 0x38000 and relocate SMBASE */
	smm_relocate();

	/* We're done. Make sure SMIs can happen! */
	smi_set_eos();
}

void smm_lock(void)
{
	/* LOCK the SMM memory window and enable normal SMM.
	 * After running this function, only a full reset can
	 * make the SMM registers writable again.
	 */
	printk(BIOS_DEBUG, "Locking SMM.\n");
	pci_write_config8(PCI_DEV(QUICKPATH_BUS, 0, 1), QPD0F1_SMRAM,
			D_LCK | G_SMRAME | C_BASE_SEG);
}

void smm_setup_structures(void *gnvs, void *tcg, void *smi1)
{
	/*
	 * Issue SMI to set the gnvs pointer in SMM.
	 * tcg and smi1 are unused.
	 *
	 * EAX = APM_CNT_GNVS_UPDATE
	 * EBX = gnvs pointer
	 * EDX = APM_CNT
	 */
	asm volatile (
		"outb %%al, %%dx\n\t"
		: /* ignore result */
		: "a" (APM_CNT_GNVS_UPDATE),
		  "b" ((u32)gnvs),
		  "d" (APM_CNT)
	);
}
