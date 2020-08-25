/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/watchdog.h>
#include <soc/clock.h>
#include <console/console.h>
#include <device/mmio.h>
#include <vendorcode/google/chromeos/chromeos.h>

void check_wdog(void)
{
	uint32_t wdog_sta = read32(&aoss->aoss_cc_reset_status);

	if (wdog_sta & WDOG_RESET_BIT_MASK)
		mark_watchdog_tombstone();
}
