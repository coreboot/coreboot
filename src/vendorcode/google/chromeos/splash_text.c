/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <stdio.h>
#include <string.h>

static bool get_battery_status_msg(char *msg, size_t msg_max)
{
	uint32_t batt_pct;

	if (google_chromeec_read_batt_state_of_charge(&batt_pct)) {
		printk(BIOS_WARNING, "Failed to get battery level\n");
		return false;
	}

	if (google_chromeec_is_critically_low_on_battery()) {
		snprintf(msg, msg_max, "%u%%", batt_pct);
	} else if (google_chromeec_is_charger_present()) {
		if (batt_pct == 100)
			snprintf(msg, msg_max, "Charged");
		else
			snprintf(msg, msg_max, "Charging: %u%%", batt_pct);
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
