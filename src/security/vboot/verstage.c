/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <arch/exception.h>
#include <arch/hlt.h>
#include <compiler.h>
#include <console/console.h>
#include <program_loading.h>
#include <security/vboot/vboot_common.h>

void __weak verstage_mainboard_init(void)
{
	/* Default empty implementation. */
}

void main(void)
{
	console_init();
	exception_init();
	verstage_mainboard_init();

	if (IS_ENABLED(CONFIG_VBOOT_RETURN_FROM_VERSTAGE)) {
		verstage_main();
	} else {
		run_romstage();
		hlt();
	}
}
