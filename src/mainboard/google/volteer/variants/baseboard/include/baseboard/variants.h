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
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_sleep_gpio_table(u8 slp_typ, size_t *num);
const struct pad_config *variant_override_gpio_table(size_t *num);

const struct mb_cfg *variant_memory_params(void);
int variant_memory_sku(void);
void memcfg_variant_init(FSPM_UPD *mupd);

/* Modify devictree settings during ramstage. */
void variant_devtree_update(void);

/* Perform variant specific initialization early on in ramstage */
void variant_ramstage_init(void);

#endif /* __BASEBOARD_VARIANTS_H__ */
