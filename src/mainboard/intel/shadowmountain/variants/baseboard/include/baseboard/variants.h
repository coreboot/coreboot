/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stddef.h>

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.
 */
const struct pad_config *variant_base_gpio_table(size_t *num);
const struct pad_config *variant_override_gpio_table(size_t *num);

void variant_configure_early_gpio_pads(void);

const struct mb_cfg *variant_memory_params(void);
int variant_memory_sku(void);

#endif /* __BASEBOARD_VARIANTS_H__ */
