/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stdint.h>

enum jsl_board_id {
	jsl_ddr4 = 1,
	jsl_lpddr4 = 4,
};

/* The next set of functions return the gpio table and fill in the number of
 * entries for each table. */

const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct mb_cfg *variant_memcfg_config(uint8_t board_id);

#endif /*__BASEBOARD_VARIANTS_H__ */
