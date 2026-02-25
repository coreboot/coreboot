/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <bootstate.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <elog.h>
#include <halt.h>

void __weak poweroff(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;
	google_chromeec_ap_poweroff();
}

static void low_battery_poweroff(void *unused)
{
	if (!platform_is_low_battery_shutdown_needed())
		return;

	printk(BIOS_INFO, "%s: powering off...\n", __func__);

	platform_handle_emergency_low_battery();
	poweroff();
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, low_battery_poweroff, NULL);
