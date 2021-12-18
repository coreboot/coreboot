/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BASEBOARD_VARIANTS_H_
#define _BASEBOARD_VARIANTS_H_

#include <soc/gpio.h>

enum cmos_power_profile {
	PP_POWER_SAVER = 0,
	PP_BALANCED    = 1,
	PP_PERFORMANCE = 2,
};
#define NUM_POWER_PROFILES 3

enum cmos_power_profile get_power_profile(enum cmos_power_profile fallback);

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.
 */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);

void devtree_update(void);

#endif /* _BASEBOARD_VARIANTS_H_ */
