/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#ifndef BASEBOARD_VARIANTS_H
#define BASEBOARD_VARIANTS_H

#include <soc/cnl_memcfg_init.h>
#include <soc/gpio.h>
#include <stdint.h>
#include <vendorcode/google/chromeos/chromeos.h>

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.  The "base" GPIOs live in the "baseboard" variant, and
 * the overrides live with the specific board (kohaku, kled, etc.).
*/
const struct pad_config *base_gpio_table(size_t *num);
const struct pad_config *override_gpio_table(size_t *num);

/* Return board specific memory configuration */
void variant_memory_params(struct cnl_mb_cfg *bcfg);

/* Return memory SKU for the variant */
int variant_memory_sku(void);

/* Return variant specific gpio pads to be configured during sleep */
const struct pad_config *variant_sleep_gpio_table(u8 slp_typ, size_t *num);

/* Return GPIO pads that need to be configured before ramstage */
const struct pad_config *variant_early_gpio_table(size_t *num);

/* Return ChromeOS gpio table and fill in number of entries. */
const struct cros_gpio *variant_cros_gpios(size_t *num);

/* Return board SKU */
uint32_t get_board_sku(void);

/* Modify devictree settings during ramstage. */
void variant_devtree_update(void);

/* Perform variant specific initialization early on in ramstage. */
void variant_ramstage_init(void);

/* Perform variant specific mainboard initialization */
void variant_mainboard_enable(struct device *dev);

#endif /* BASEBOARD_VARIANTS_H */
