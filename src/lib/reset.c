/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google, Inc.
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

#include <arch/cache.h>
#include <compiler.h>
#include <console/console.h>
#include <halt.h>
#include <reset.h>

__noreturn static void __hard_reset(void) {
	if (IS_ENABLED(CONFIG_HAVE_HARD_RESET))
		do_hard_reset();
	else
		printk(BIOS_CRIT, "No hard_reset implementation, hanging...\n");
	halt();
}

/* Not all platforms implement all reset types. Fall back to hard_reset. */
__weak void do_global_reset(void) { __hard_reset(); }
__weak void do_soft_reset(void) { __hard_reset(); }

__weak void soc_reset_prepare(enum reset_type rt) { /* no-op */ }

void global_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	soc_reset_prepare(GLOBAL_RESET);
	dcache_clean_all();
	do_global_reset();
	halt();
}

void hard_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	soc_reset_prepare(HARD_RESET);
	dcache_clean_all();
	__hard_reset();
}

void soft_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	soc_reset_prepare(SOFT_RESET);
	dcache_clean_all();
	do_soft_reset();
	halt();
}
