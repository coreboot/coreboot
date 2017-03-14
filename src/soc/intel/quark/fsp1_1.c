/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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
#include <soc/ramstage.h>

void fsp_silicon_init(bool s3wake)
{
	if (IS_ENABLED(CONFIG_RELOCATE_FSP_INTO_DRAM))
		intel_silicon_init();
	else
		fsp_run_silicon_init(find_fsp(CONFIG_FSP_ESRAM_LOC), s3wake);
}

void soc_silicon_init_params(SILICON_INIT_UPD *upd)
{
}

void soc_display_silicon_init_params(const SILICON_INIT_UPD *old,
	SILICON_INIT_UPD *new)
{
}
