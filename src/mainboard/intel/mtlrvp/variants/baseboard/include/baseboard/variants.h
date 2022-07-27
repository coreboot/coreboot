/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/meminit.h>
#include <stdint.h>

enum mtl_boardid {
	MTLP_DDR5_RVP = 0x01,
	MTLP_LP5_T3_RVP = 0x02,
	MTLP_LP5_T4_RVP = 0x04,
	MTLM_LP5_RVP = 0x06,
};

/* Functions to configure GPIO */
void configure_early_gpio_pads(void);
void configure_gpio_pads(void);

/* Function to initialize memory params based on variant */
const struct mb_cfg *variant_memory_params(void);

#endif /*__BASEBOARD_VARIANTS_H__ */
