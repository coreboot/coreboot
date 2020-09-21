/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <halt.h>
#include <reset.h>

#include "reset.h"

void global_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	cf9_reset_prepare();
	dcache_clean_all();
	do_global_reset();
	halt();
}

void do_board_reset(void)
{
	full_reset();
}
