/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
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

#include "chip.h"
#include <console/console.h>
#include <fsp/ramstage.h>

static void soc_init(void *chip_info)
{
	/* Perform silicon specific init. */
	if (IS_ENABLED(CONFIG_RELOCATE_FSP_INTO_DRAM))
		intel_silicon_init();
	else
		fsp_run_silicon_init(find_fsp(CONFIG_FSP_ESRAM_LOC), 0);
}

struct chip_operations soc_intel_quark_ops = {
	CHIP_NAME("Intel Quark")
	.init		= &soc_init,
};
