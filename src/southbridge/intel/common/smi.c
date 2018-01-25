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


#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <arch/io.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/smm/gen1/smi.h>

#include "pmutil.h"

#define DEBUG_PERIODIC_SMIS 0

static u16 pmbase;

u16 get_pmbase(void)
{
	return pmbase;
}

void southbridge_smm_init(void)
{
	u32 smi_en;
	u16 pm1_en;
	u32 gpe0_en;

#if IS_ENABLED(CONFIG_ELOG)
	/* Log events from chipset before clearing */
	pch_log_state();
#endif

	printk(BIOS_DEBUG, "Initializing southbridge SMI...");

	pmbase = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x1f, 0)),
						D31F0_PMBASE) & 0xff80;

	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", pmbase);

	smi_en = inl(pmbase + SMI_EN);
	if (smi_en & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

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
}

void southbridge_trigger_smi(void)
{
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

void southbridge_clear_smi_status(void)
{
	/* Clear SMI status */
	reset_smi_status();

	/* Clear PM1 status */
	reset_pm1_status();

	/* Set EOS bit so other SMIs can occur. */
	smi_set_eos();
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

void southbridge_smm_clear_state(void)
{
	u32 smi_en;

	if (IS_ENABLED(CONFIG_ELOG))
	/* Log events from chipset before clearing */
		pch_log_state();

	printk(BIOS_DEBUG, "Initializing Southbridge SMI...");
	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", get_pmbase());

	smi_en = inl(get_pmbase() + SMI_EN);
	if (smi_en & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	printk(BIOS_DEBUG, "\n");

	/* Dump and clear status registers */
	reset_smi_status();
	reset_pm1_status();
	reset_tco_status();
	reset_gpe0_status();
}
