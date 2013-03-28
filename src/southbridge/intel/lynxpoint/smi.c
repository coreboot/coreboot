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


#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <arch/io.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <string.h>
#include "pch.h"

void southbridge_smm_clear_state(void)
{
	u32 smi_en;

#if CONFIG_ELOG
	/* Log events from chipset before clearing */
	pch_log_state();
#endif
	printk(BIOS_DEBUG, "Initializing Southbridge SMI...");
	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", get_pmbase());

	smi_en = inl(get_pmbase() + SMI_EN);
	if (smi_en & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	printk(BIOS_DEBUG, "\n");

	/* Dump and clear status registers */
	clear_smi_status();
	clear_pm1_status();
	clear_tco_status();
	clear_gpe_status();
}

void southbridge_smm_enable_smi(void)
{
	printk(BIOS_DEBUG, "Enabling SMIs.\n");
	/* Configure events */
	enable_pm1(PWRBTN_EN | GBL_EN);
	disable_gpe(PME_B0_EN);

	/* Enable SMI generation:
	 *  - on TCO events
	 *  - on APMC writes (io 0xb2)
	 *  - on writes to SLP_EN (sleep states)
	 *  - on writes to GBL_RLS (bios commands)
	 * No SMIs:
	 *  - on microcontroller writes (io 0x62/0x66)
	 */
	enable_smi(TCO_EN | APMC_EN | SLP_SMI_EN | GBL_SMI_EN | EOS);
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
	clear_smi_status();

	/* Clear PM1 status */
	clear_pm1_status();

	/* Set EOS bit so other SMIs can occur. */
	enable_smi(EOS);
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
