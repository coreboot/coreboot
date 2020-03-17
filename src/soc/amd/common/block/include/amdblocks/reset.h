/*
 * This file is part of the coreboot project.
 *
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

#ifndef __AMD_RESET_H__
#define __AMD_RESET_H__

#include <console/console.h>
#include <arch/cache.h>
#include <halt.h>

void do_warm_reset(void);
void do_cold_reset(void);

static inline __noreturn void warm_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	dcache_clean_all();
	do_warm_reset();
	halt();
}

static inline __noreturn void cold_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	dcache_clean_all();
	do_cold_reset();
	halt();
}

#endif /* __AMD_RESET_H__ */
