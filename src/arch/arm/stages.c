/*
 * This file is part of the coreboot project.
 *
 * Copyright 2012 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

/*
 * This file contains entry/exit functions for each stage during coreboot
 * execution (bootblock entry and ramstage exit will depend on external
 * loading).
 *
 * Entry points should be set in the linker script and honored by CBFS,
 * so text section layout shouldn't matter. Still, it doesn't hurt to put
 * stage_entry first (which XXXstage.ld will do automatically through the
 * .text.stage_entry section created by -ffunction-sections).
 */

#include <arch/stages.h>
#include <arch/cache.h>

/**
 * generic stage entry point. override this if board specific code is needed.
 */
__attribute__((weak)) void stage_entry(void)
{
	main();
}

/* we had marked 'doit' as 'noreturn'.
 * There is no apparent harm in leaving it as something we can return from, and in the one
 * case where we call a payload, the payload is allowed to return.
 * Hence, leave it as something we can return from.
 */
void stage_exit(void *addr)
{
	void (*doit)(void) = addr;
	/*
	 * Most stages load code so we need to sync caches here. Should maybe
	 * go into cbfs_load_stage() instead...
	 */
	cache_sync_instructions();
	doit();
}
