/* SPDX-License-Identifier: GPL-2.0-only */

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
