/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BASEBOARD_VARIANTS_H_
#define _BASEBOARD_VARIANTS_H_

#include <gpio.h>

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.
 */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);

/* This function provides the swizzle data for the DRAM initialization. */
const struct lpddr4_swizzle_cfg *variant_lpddr4_swizzle_config(void);

/* The following function performs board specific things. */
void variant_mainboard_final(void);

#endif /* _BASEBOARD_VARIANTS_H_ */
