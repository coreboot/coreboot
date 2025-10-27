/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _STARLABS_CMN_POWERCAP_H_
#define _STARLABS_CMN_POWERCAP_H_

enum cmos_power_profile {
	PP_POWER_SAVER	= 0,
	PP_BALANCED	= 1,
	PP_PERFORMANCE	= 2,
};
#define NUM_POWER_PROFILES 3

enum cmos_power_profile get_power_profile(enum cmos_power_profile fallback);

#endif
