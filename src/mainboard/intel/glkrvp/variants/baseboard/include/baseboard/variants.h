/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BASEBOARD_VARIANTS_H
#define BASEBOARD_VARIANTS_H

#include <soc/gpio.h>
#include <soc/meminit.h>

/**
 * variant_board_id() - Get the board id for the current board variant
 *
 * Return: board id on success, -1 on failure/error.
 */
int variant_board_id(void);

/* The next set of functions return the gpio table and fill in the number of
 * entries for each table. */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_sleep_gpio_table(size_t *num);

/* Baseboard default swizzle. Can be reused if swizzle is same. */
extern const struct lpddr4_swizzle_cfg baseboard_lpddr4_swizzle;
/* Return LPDDR4 configuration structure. */
const struct lpddr4_cfg *variant_lpddr4_config(void);
/* Return memory SKU for the board. */
size_t variant_memory_sku(void);

/* Seed the NHLT tables with the board specific information. */
struct nhlt;
void variant_nhlt_init(struct nhlt *nhlt);

#endif /* BASEBOARD_VARIANTS_H */
