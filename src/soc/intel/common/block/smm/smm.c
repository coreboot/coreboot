/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
 */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <intelblocks/smm.h>
#include <soc/pm.h>

void smm_southbridge_clear_state(void)
{
	printk(BIOS_DEBUG, "Clearing SMI status registers\n");

	if (get_smi_en() & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	/* Dump and clear status registers */
	clear_smi_status();
	clear_pm1_status();
	clear_tco_status();
	clear_gpe_status();
}

void smm_southbridge_enable(void)
{
	printk(BIOS_DEBUG, "Enabling SMIs.\n");
	/* Configure events */
	enable_pm1(PWRBTN_EN | GBL_EN);
	disable_gpe(PME_B0_EN);

	/*
	 * Enable SMI generation:
	 *  - on APMC writes (io 0xb2)
	 *  - on writes to SLP_EN (sleep states)
	 *  - on writes to GBL_RLS (bios commands)
	 *  - on eSPI events (does nothing on LPC systems)
	 * No SMIs:
	 *  - on microcontroller writes (io 0x62/0x66)
	 *  - on TCO events
	 */

	/* Enable SMI generation: */
	enable_smi(ENABLE_SMI_PARAMS);
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
