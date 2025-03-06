/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <bootsplash.h>

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
