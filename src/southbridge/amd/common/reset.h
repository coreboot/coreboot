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

#ifndef _AMD_SB_RESET_H_
#define _AMD_SB_RESET_H_

#include <arch/cache.h>
#include <console/console.h>
#include <halt.h>

/* Implement the bare reset, e.g. write to cf9. */
void do_soft_reset(void);

/* Prepare for reset, run do_soft_reset(), halt. */
static inline __noreturn void soft_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	dcache_clean_all();
	do_soft_reset();
	halt();
}

#endif	/* _AMD_SB_RESET_H_ */
