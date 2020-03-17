/*
 * This file is part of the coreboot project.
 *
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

#include <arch/romstage.h>
#include <cbmem.h>
#include <intelblocks/rtc.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <soc/romstage.h>
#include <soc/soc_util.h>
#include "chip.h"

asmlinkage void car_stage_entry(void)
{
	struct postcar_frame pcf;
	uintptr_t top_of_ram;

	printk(BIOS_DEBUG, "FSP TempRamInit was successful...\n");

	console_init();
	rtc_init();

	fsp_memory_init(false);
	printk(BIOS_DEBUG, "coreboot fsp_memory_init finished...\n");

	unlock_pam_regions();

	if (postcar_frame_init(&pcf, 1 * KiB))
		die("Unable to initialize postcar frame.\n");

	/*
	 * We need to make sure ramstage will be run cached. At this point exact
	 * location of ramstage in cbmem is not known. Instruct postcar to cache
	 * 16 megs under cbmem top which is a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t)cbmem_top();
	printk(BIOS_DEBUG, "top_of_ram: 0x%lx\n", top_of_ram);
	postcar_frame_add_mtrr(&pcf, top_of_ram - 16 * MiB, 16 * MiB,
		MTRR_TYPE_WRBACK);

	/* Cache the memory-mapped boot media. */
	postcar_frame_add_romcache(&pcf, MTRR_TYPE_WRPROT);

	run_postcar_phase(&pcf);
}

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg)
{
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const config_t *config = config_of_soc();
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	mupd->FspmUpdVersion = FSP_UPD_VERSION;

	// ErrorLevel - 0 (disable) to 8 (verbose)
	m_cfg->PcdFspMrcDebugPrintErrorLevel = 0;
	m_cfg->PcdFspKtiDebugPrintErrorLevel = 0;

	soc_memory_init_params(m_cfg);

	mainboard_memory_init_params(mupd);

	m_cfg->VTdConfig.VTdSupport = config->vtd_support;
	m_cfg->VTdConfig.CoherencySupport = config->coherency_support;
	m_cfg->VTdConfig.ATS = config->ats_support;
}
