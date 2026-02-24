/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <bootsplash.h>
#include <elog.h>
#include <delay.h>

/*
 * Check if low battery shutdown is needed
 *
 * This function checks if the system needs to shut down due to a critically low
 * battery level. It performs the following actions:
 *
 * 1. If Chrome EC is not supported, returns false (no shutdown needed).
 * 2. Uses static variables to cache the result and avoid repeated checks.
 * 3. If the battery level has not been checked yet:
 *    - Queries the Chrome EC to determine if the battery is below the critical threshold.
 *    - If the battery is below the threshold, sets the result to true.
 * 4. Returns the cached result.
 */
bool platform_is_low_battery_shutdown_needed(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return false;

	static bool result = false;
	static bool checked = false;

	if (!checked) {
		if (google_chromeec_is_below_critical_threshold())
			result = true;
		checked = true;
	}

	return result;
}

/*
 * Platform hooks for system shutdown due to critical battery levels.
 * Provides visual feedback via the Lightbar/LEDs and logs the event
 * to non-volatile storage before signaling to cut power.
 */
void platform_handle_emergency_low_battery(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	/* Visual alert: Set Lightbar to solid Red */
	google_chromeec_set_lightbar_rgb(0xff, 0xff, 0x00, 0x00);

	/* Record the event for post-mortem diagnostics (stored in CMOS/Flash) */
	elog_add_event_byte(ELOG_TYPE_LOW_BATTERY_INDICATOR, ELOG_FW_ISSUE_SHUTDOWN);

	/* * Pause briefly to ensure the user perceives the LED change and
	 * the event log is safely committed to storage.
	 */
	delay(CONFIG_PLATFORM_POST_RENDER_DELAY_SEC);
}
