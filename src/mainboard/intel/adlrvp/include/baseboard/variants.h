/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stdint.h>

enum adl_boardid {
	/* ADL-P LPDDR4 RVPs */
	ADL_P_LP4_1 = 0x10,
	ADL_P_LP4_2 = 0x11,
	/* ADL-P DDR5 RVPs */
	ADL_P_DDR5_1 = 0x12,
	ADL_P_DDR5_2 = 0x16,
	/* ADL-P LPDDR5 RVP */
	ADL_P_LP5_1 = 0x13,
	ADL_P_LP5_2 = 0x17,
	/* ADL-P DDR4 RVPs */
	ADL_P_DDR4_1 = 0x14,
	ADL_P_DDR4_2 = 0x3F,
	/* ADL-M LP4 and LP5 RVPs */
	ADL_M_LP4 = 0x1,
	ADL_M_LP5 = 0x2,
	/* ADL-N LP5 RVP */
	ADL_N_LP5 = 0x7,
};

/* Functions to configure GPIO as per variant schematics */
void variant_configure_gpio_pads(void);
void variant_configure_early_gpio_pads(void);

size_t variant_memory_sku(void);
const struct mb_cfg *variant_memory_params(void);

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
