/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_SPD_H

#include <fsp/soc_binding.h>
#include <gpio.h>
#include "../gpio.h"

#define MAINBOARD_SPD_H

#define HYNIX_SINGLE_CHAN	0x1
#define SAMSUNG_SINGLE_CHAN	0x4
#define MIC_SINGLE_CHAN		0x5

/* PCH_MEM_CFG[3:0] */
#define MAX_MEMORY_CONFIG	0x10
#define K4E6E304EE_MEM_ID	0x3

static inline int get_spd_index(void) {
	/* PCH_MEM_CFG[3:0] */
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};
	return (gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios)));
}
void mainboard_fill_dq_map_data(void *dq_map_ch0, void *dq_map_ch1);
void mainboard_fill_dqs_map_data(void *dqs_map_ch0, void *dqs_map_ch1);
void mainboard_fill_rcomp_res_data(void *rcomp_ptr);
void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr);
uintptr_t mainboard_get_spd_data(void);
int mainboard_has_dual_channel_mem(void);
#endif
