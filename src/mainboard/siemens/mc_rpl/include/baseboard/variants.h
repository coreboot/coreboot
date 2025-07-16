/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stdint.h>

/* Functions to configure GPIO as per variant schematics */
void variant_configure_gpio_pads(void);
void variant_configure_early_gpio_pads(void);

size_t variant_memory_sku(void);
const struct mb_cfg *variant_memory_params(void);
void rpl_memory_params(FSPM_UPD *memupd);
void variant_configure_fspm(FSPM_UPD *memupd);

#endif /*__BASEBOARD_VARIANTS_H__ */
