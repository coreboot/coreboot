/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <bootsplash.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
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
	platform_handle_emergency_low_battery();

	/*
	 * Disable lightbar for visual consistency between the built-in
	 * display indicators and the external LED status.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC_LED_CONTROL))
		google_chromeec_lightbar_off();

	poweroff();
	halt();
}
