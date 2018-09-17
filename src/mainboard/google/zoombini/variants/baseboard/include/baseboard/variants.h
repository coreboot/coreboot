/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
 * Copyright 2017 Intel Corporation.
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

#include <soc/cnl_memcfg_init.h>
#include <soc/gpio.h>
#include <stdint.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Return the board id for the current variant board. */
int variant_board_id(void);

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.
 */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);

const struct cros_gpio *variant_cros_gpios(size_t *num);

/* Return LPDDR4 configuration structure. */
const struct cnl_mb_cfg *variant_lpddr4_config(void);

/* Return memory SKU for the board. */
size_t variant_memory_sku(void);

/* Seed the NHLT tables with the board specific information. */
struct nhlt;
void variant_nhlt_init(struct nhlt *nhlt);

#endif /*__BASEBOARD_VARIANTS_H__ */
