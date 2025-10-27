/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _STARLABS_CMN_POWERCAP_H_
#define _STARLABS_CMN_POWERCAP_H_

#include <soc/soc_chip.h>

#define TCC(temp)	(CONFIG_TJ_MAX - temp)

enum cmos_power_profile {
	PP_POWER_SAVER	= 0,
	PP_BALANCED	= 1,
	PP_PERFORMANCE	= 2,
};
#define NUM_POWER_PROFILES 3

void update_power_limits(config_t *cfg);

#endif
