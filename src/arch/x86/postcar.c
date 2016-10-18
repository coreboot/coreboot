/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google, Inc.
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

#include <cbmem.h>
#include <console/console.h>
#include <main_decl.h>
#include <program_loading.h>
#include <soc/intel/common/util.h>
#include <fsp/util.h>

void main(void)
{
	/* Call TempRamExit FSP API if enabled. */
	if (IS_ENABLED(CONFIG_FSP_CAR))
		fsp_temp_ram_exit();

	console_init();

	/* Recover cbmem so infrastruture using it is functional. */
	cbmem_initialize();

	/* Display the MTRRs */
	if (IS_ENABLED(CONFIG_DISPLAY_MTRRS))
		soc_display_mtrrs();

	/* Load and run ramstage. */
	run_ramstage();
}
