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

#include <arch/cache.h>
#include <console/console.h>
#include <halt.h>
#include <reset.h>

__noreturn void board_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	dcache_clean_all();
	do_board_reset();
	halt();
}

#if CONFIG(MISSING_BOARD_RESET)
void do_board_reset(void)
{
	printk(BIOS_CRIT, "No board_reset implementation, hanging...\n");
}
#endif
