/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/iomap.h>
#include <reset.h>

/* Watchdog bite time set to default reset value */
#define RESET_WDT_BITE_TIME 0x31F3

/* Watchdog bark time value is kept larger than the watchdog timeout
 * of 0x31F3, effectively disabling the watchdog bark interrupt
 */
#define RESET_WDT_BARK_TIME (5 * RESET_WDT_BITE_TIME)

static void wdog_reset(void)
{
	printk(BIOS_DEBUG, "\nResetting with watchdog!\n");

	write32(APCS_WDT0_EN, 0);
	write32(APCS_WDT0_RST, 1);
	write32(APCS_WDT0_BARK_TIME, RESET_WDT_BARK_TIME);
	write32(APCS_WDT0_BITE_TIME, RESET_WDT_BITE_TIME);
	write32(APCS_WDT0_EN, 1);
	write32(APCS_WDT0_CPU0_WDOG_EXPIRED_ENABLE, 1);
}

void do_board_reset(void)
{
	wdog_reset();
}
