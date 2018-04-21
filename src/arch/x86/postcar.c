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

#include <arch/cpu.h>
#include <cbmem.h>
#include <compiler.h>
#include <console/console.h>
#include <main_decl.h>
#include <program_loading.h>
#include <soc/intel/common/util.h>

/*
 * Systems without a native coreboot cache-as-ram teardown may implement
 * this to use an alternate method.
 */
__weak void late_car_teardown(void) { /* do nothing */ }

void main(void)
{
	late_car_teardown();

	console_init();

	/* Recover cbmem so infrastruture using it is functional. */
	cbmem_initialize();

	/* Display the MTRRs */
	if (IS_ENABLED(CONFIG_DISPLAY_MTRRS))
		soc_display_mtrrs();

	/* Load and run ramstage. */
	run_ramstage();
}
