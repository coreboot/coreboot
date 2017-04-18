/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <intelblocks/pmclib.h>
#include <string.h>
#include <soc/pm.h>
#include <soc/smm.h>

void southbridge_smm_clear_state(void)
{
	printk(BIOS_DEBUG, "Initializing Southbridge SMI...");

	if (pmc_get_smi_en() & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	printk(BIOS_DEBUG, "Done\n");

	/* Dump and clear status registers */
	pmc_clear_smi_status();
	pmc_clear_pm1_status();
	pmc_clear_tco_status();
	pmc_clear_gpe_status();
}

void southbridge_smm_enable_smi(void)
{
	printk(BIOS_DEBUG, "Enabling SMIs.\n");
	/* Configure events */
	pmc_enable_pm1(PWRBTN_EN | GBL_EN);
	pmc_disable_gpe(PME_B0_EN);

	/* Enable SMI generation */
	pmc_enable_smi(APMC_EN | SLP_SMI_EN | GBL_SMI_EN | EOS | GPIO_EN);
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
