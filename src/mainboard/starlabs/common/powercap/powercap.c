/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <common/powercap.h>

enum cmos_power_profile get_power_profile(enum cmos_power_profile fallback)
{
	const unsigned int power_profile = get_uint_option("power_profile", fallback);
	return power_profile < NUM_POWER_PROFILES ? power_profile : fallback;
}
