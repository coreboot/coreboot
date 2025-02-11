/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <delay.h>
#include <elog.h>
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

void do_low_battery_poweroff(void)
{
	if (CONFIG(PLATFORM_HAS_LOW_BATTERY_INDICATOR)) {
		elog_add_event_byte(ELOG_TYPE_LOW_BATTERY_INDICATOR, ELOG_FW_ISSUE_SHUTDOWN);
		delay(CONFIG_PLATFORM_LOW_BATTERY_SHUTDOWN_DELAY_SEC);
	}

	poweroff();
}
