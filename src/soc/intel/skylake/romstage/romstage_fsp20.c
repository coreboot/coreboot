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

#include <console/console.h>
#include <fsp/util.h>
#include <soc/romstage.h>

asmlinkage void *car_stage_c_entry(void)
{
	bool s3wake = false;
	console_init();
	/* TODO: Add fill_powerstate and determine sleep state. */
	fsp_memory_init(s3wake);
	return NULL;
}
static void soc_memory_init_params(struct FSP_M_CONFIG *m_cfg)
{
	/* TODO: Fill SoC specific Memory init Params */
}

void platform_fsp_memory_init_params_cb(struct FSPM_UPD *mupd){

	struct FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	soc_memory_init_params(m_cfg);
	mainboard_memory_init_params(mupd);
}

__attribute__((weak)) void mainboard_memory_init_params(struct FSPM_UPD *mupd)
{
	/* Do nothing */
}
