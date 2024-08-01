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

/* Modify devictree settings during ramstage */
void variant_devtree_update(void);
struct cpu_power_limits {
	uint16_t mchid;
	u8 cpu_tdp;
	unsigned int pl1_min_power;
	unsigned int pl1_max_power;
	unsigned int pl2_min_power;
	unsigned int pl2_max_power;
	unsigned int pl4_power;
};
/* Modify Power Limit devictree settings during ramstage */
void variant_update_power_limits(void);

#endif /*__BASEBOARD_VARIANTS_H__ */
