/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <stdio.h>
#include <string.h>

/*
 * EC reserved 4% battery fuel as residue to avoid entering into critical battery.
 * Therefore, while showing user visible battery SOC as part of low-battery or off-mode
 * charging, need to consider this logic. Otherwise, user visible battery SOC would
 * differ between FW and OS.
 */
#define MIN_BATTERY_THRESHOLD_FOR_CRITICAL_BOOT 4

static bool get_battery_status_msg(char *msg, size_t msg_max)
{
	uint32_t batt_pct;
	uint32_t user_batt_pct;

	if (google_chromeec_read_batt_state_of_charge(&batt_pct)) {
		printk(BIOS_WARNING, "Failed to get battery level\n");
		return false;
	}

	/*
	 * Scale the raw EC battery percentage to user-visible percentage.
	 * If raw battery is below or equal to the threshold, user sees 0%.
	 */
	if (batt_pct <= MIN_BATTERY_THRESHOLD_FOR_CRITICAL_BOOT) {
		user_batt_pct = 0;
	} else {
		user_batt_pct = ((batt_pct - MIN_BATTERY_THRESHOLD_FOR_CRITICAL_BOOT) * 100) /
				 (100 - MIN_BATTERY_THRESHOLD_FOR_CRITICAL_BOOT);
	}

	if (google_chromeec_is_critically_low_on_battery()) {
		snprintf(msg, msg_max, "%u%%", user_batt_pct);
	} else if (google_chromeec_is_charger_present()) {
		if (user_batt_pct == 100)
			snprintf(msg, msg_max, "Charged");
		else
			snprintf(msg, msg_max, "Charging: %u%%", user_batt_pct);
	}

	return true;
}

bool platform_get_splash_text(enum bootsplash_type logo_type, char *msg, size_t msg_max)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC) || !msg || msg_max == 0)
		return false;

	memset(msg, 0, msg_max);

	if (logo_type == BOOTSPLASH_LOW_BATTERY || logo_type == BOOTSPLASH_OFF_MODE_CHARGING)
		return get_battery_status_msg(msg, msg_max);

	return false;
}
