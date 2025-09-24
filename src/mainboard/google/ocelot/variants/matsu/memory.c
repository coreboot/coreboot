/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  10,  11,  8,  9,  15,  14,  12,  13, },
			.dq1 = {  7,  4,  5,  6,  3,  0,  2,  1 },
		},
		.ddr1 = {
			.dq0 = {  8,  11,  10,  9,  15,  12,  13,  14, },
			.dq1 = {  6,  7,  5,  4,  1,  3,  0,  2 },
		},
		.ddr2 = {
			.dq0 = {  3,  2,  1,  0,  6,  7,  5,  4, },
			.dq1 = {  9,  8,  10,  11,  15,  12,  13,  14 },
		},
		.ddr3 = {
			.dq0 = {  8,  11,  10,  9,  15,  13,  12,  14, },
			.dq1 = {  4,  5,  6,  7,  1,  3,  2,  0 },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
	},

	.ect = true, /* Early Command Training */

	.user_bd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0xFF,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &lp5_mem_config;
}

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * GPIO_MEM_CONFIG_0	GPP_H23
	 * GPIO_MEM_CONFIG_1	GPP_H24
	 * GPIO_MEM_CONFIG_2	GPP_B06
	 * GPIO_MEM_CONFIG_3    GPP_B08
	 */
	gpio_t spd_gpios[] = {
		GPP_H23,
		GPP_H24,
		GPP_B06,
		GPP_B08,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}
