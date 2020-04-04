/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/cache.h>
#include <soc/power.h>
#include <soc/wakeup.h>

void wakeup(void)
{
	if (wakeup_need_reset())
		power_reset();

	power_init();  /* Ensure ps_hold_setup() for early wakeup. */
	dcache_mmu_disable();
	power_exit_wakeup();
	/* Should never return. If we do, reset. */
	power_reset();
}

int get_wakeup_state(void)
{
	uint32_t status = power_read_reset_status();

	/* DIDLE/LPA can be resumed without clock reset (ex, bootblock),
	 * and SLEEP requires resetting clock (should be done in ROM stage).
	 */

	if (status == S5P_CHECK_DIDLE || status == S5P_CHECK_LPA)
		return WAKEUP_DIRECT;

	if (status == S5P_CHECK_SLEEP)
		return WAKEUP_NEED_CLOCK_RESET;

	return IS_NOT_WAKEUP;
}

void wakeup_enable_uart(void)
{
	power_release_uart_retention();
}
