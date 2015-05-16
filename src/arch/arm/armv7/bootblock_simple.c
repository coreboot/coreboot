/*
 * This file is part of the coreboot project.
 *
 * Copyright 2010 Google Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/exception.h>
#include <arch/stages.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <smp/node.h>
#include <timestamp.h>

__attribute__((weak)) void bootblock_mainboard_early_init(void) { /* no-op */ }
__attribute__((weak)) void bootblock_soc_init(void) { /* do nothing */ }
__attribute__((weak)) void bootblock_mainboard_init(void) { /* do nothing */ }

void main(void)
{
	init_timer();
	if (IS_ENABLED(CONFIG_HAS_PRECBMEM_TIMESTAMP_REGION))
		timestamp_init(timestamp_get());

	bootblock_mainboard_early_init();

#if CONFIG_BOOTBLOCK_CONSOLE
	console_init();
	exception_init();
#endif

	bootblock_soc_init();
	bootblock_mainboard_init();

	run_romstage();
}
