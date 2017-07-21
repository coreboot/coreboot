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
	struct chipset_power_state *ps;

	console_init();

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();

	ps = fill_power_state();
	timestamp_add_now(TS_START_ROMSTAGE);
	s3wake = ps->prev_sleep_state == ACPI_S3;
	fsp_memory_init(s3wake);
	die("Get out from FSP memoryinit. \n");
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	mainboard_memory_init_params(mupd);
}

__attribute__((weak)) void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Do nothing */
}