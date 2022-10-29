/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BASEBOARD_VARIANTS_H
#define BASEBOARD_VARIANTS_H

#include <soc/cnl_memcfg_init.h>
#include <soc/gpio.h>
#include <stdint.h>

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

/* Return GPIO pads that need to be configured after ramstage */
const struct pad_config *variant_finalize_gpio_table(size_t *num);

/* Modify devictree settings during ramstage. */
void variant_devtree_update(void);

/* Perform variant specific initialization early on in ramstage. */
void variant_ramstage_init(void);

/* Perform variant specific mainboard finalization */
void variant_final(void);

/* Perform variant specific mainboard initialization */
void variant_mainboard_enable(struct device *dev);

#endif /* BASEBOARD_VARIANTS_H */
