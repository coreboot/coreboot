/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BASEBOARD_VARIANTS_H
#define BASEBOARD_VARIANTS_H

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stdint.h>

/* Return the sku id based off the strapping resistors attached to SoC. */
uint8_t sku_strapping_value(void);

/* The next set of functions return the gpio table and fill in the number of
 * entries for each table. */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_sleep_gpio_table(u8 slp_typ, size_t *num);
const struct pad_config *variant_sku_gpio_table(size_t *num);

/* Baseboard default swizzle. Can be reused if swizzle is same. */
extern const struct lpddr4_swizzle_cfg baseboard_lpddr4_swizzle;
/* Return LPDDR4 configuration structure. */
const struct lpddr4_cfg *variant_lpddr4_config(void);
/* Return memory SKU for the board. */
size_t variant_memory_sku(void);
/* Return board SKU. Limited to uint8_t, so it fits into 3 decimal digits */
uint8_t variant_board_sku(void);
/* Set variant board sku to ec by sku id */
void variant_board_ec_set_skuid(void);

/* Seed the NHLT tables with the board specific information. */
struct nhlt;
void variant_nhlt_oem_overrides(const char **oem_id,
			const char **oem_table_id, uint32_t *oem_revision);
void variant_nhlt_init(struct nhlt *nhlt);

#endif /* BASEBOARD_VARIANTS_H */
