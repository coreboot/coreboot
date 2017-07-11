/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
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

#include <arch/io.h>
#include <arch/symbols.h>
#include <assert.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <memory_info.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <timestamp.h>

asmlinkage void car_stage_entry(void)
{
	bool s3wake;
	struct postcar_frame pcf;
	uintptr_t top_of_ram;
	struct chipset_power_state *ps;

	console_init();

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();

	ps = fill_power_state();
	timestamp_add_now(TS_START_ROMSTAGE);
	s3wake = ps->prev_sleep_state == ACPI_S3;
	fsp_memory_init(s3wake);
	if (postcar_frame_init(&pcf, 1 * KiB))
		die("Unable to initialize postcar frame.\n");

	/*
	 * We need to make sure ramstage will be run cached. At this
	 * point exact location of ramstage in cbmem is not known.
	 * Instruct postcar to cache 16 megs under cbmem top which is
	 * a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t) cbmem_top();
	printk(BIOS_DEBUG, "top_of_ram = 0x%lx\n", top_of_ram);
	top_of_ram -= 16*MiB;
	postcar_frame_add_mtrr(&pcf, top_of_ram, 16*MiB, MTRR_TYPE_WRBACK);

	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_mtrr(&pcf, 0xFFFFFFFF - CONFIG_ROM_SIZE + 1,
				CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);

	run_postcar_phase(&pcf);
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	mainboard_memory_init_params(mupd);
}

__attribute__((weak)) void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Do nothing */
}