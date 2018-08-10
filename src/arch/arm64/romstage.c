/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/exception.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <timestamp.h>

__weak void platform_romstage_main(void) { /* no-op, for bring-up */ }
__weak void platform_romstage_postram(void) { /* no-op */ }

void main(void)
{
	timestamp_add_now(TS_START_ROMSTAGE);

	console_init();
	exception_init();

	platform_romstage_main();
	cbmem_initialize_empty();
	platform_romstage_postram();

	run_ramstage();
}
