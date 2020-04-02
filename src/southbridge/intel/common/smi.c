/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */


#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <arch/io.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/smm_reloc.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/common/pmutil.h>

#include "pmutil.h"

#define DEBUG_PERIODIC_SMIS 0

u16 get_pmbase(void)
{
	return lpc_get_pmbase();
}

void smm_southbridge_enable_smi(void)
{
	u32 smi_en;
	u16 pm1_en;
	u32 gpe0_en;

	if (CONFIG(ELOG))
	/* Log events from chipset before clearing */
		pch_log_state();

	printk(BIOS_DEBUG, "Initializing southbridge SMI...");

	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", lpc_get_pmbase());

	smi_en = read_pmbase32(SMI_EN);
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
	gpe0_en = read_pmbase32(GPE0_EN);
	gpe0_en &= ~PME_B0_EN;
	write_pmbase32(GPE0_EN, gpe0_en);

	pm1_en = 0;
	pm1_en |= PWRBTN_EN;
	pm1_en |= GBL_EN;
	write_pmbase16(PM1_EN, pm1_en);

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

	write_pmbase32(SMI_EN, smi_en);
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
		  "b" ((uintptr_t)gnvs),
		  "d" (APM_CNT)
	);
}

void smm_southbridge_clear_state(void)
{
	u32 smi_en;

	if (CONFIG(ELOG))
	/* Log events from chipset before clearing */
		pch_log_state();

	printk(BIOS_DEBUG, "Initializing Southbridge SMI...\n");
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
