/*
 * This file is part of the coreboot project.
 *
 * Copyright 2020 The coreboot project Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <soc/meminit_jsl.h>
#include <stdint.h>
#include <vendorcode/google/chromeos/chromeos.h>

enum jsl_board_id {
	jsl_ddr4 = 1,
	jsl_lpddr4 = 4,
};

/* The next set of functions return the gpio table and fill in the number of
 * entries for each table. */

const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct cros_gpio *variant_cros_gpios(size_t *num);
const struct mb_cfg *variant_memcfg_config(uint8_t board_id);

#endif /*__BASEBOARD_VARIANTS_H__ */
