/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP5X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = { 40, 36, 35, 35, 35 },
	},

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  3,  2,  1,  0,  5,  4,  6,  7, },
			.dq1 = { 15, 14, 12, 13,  8,  9, 10, 11, },
		},
		.ddr1 = {
			.dq0 = {  0,  2,  3,  1,  5,  7,  4,  6, },
			.dq1 = { 14, 13, 15, 12,  8,  9, 11, 10, },
		},
		.ddr2 = {
			.dq0 = {  1,  2,  0,  3,  4,  6,  5,  7, },
			.dq1 = { 15, 13, 12, 14,  9, 10,  8, 11, },
		},
		.ddr3 = {
			.dq0 = {  2,  1,  3,  0,  7,  4,  5,  6, },
			.dq1 = { 13, 12, 15, 14,  9, 11,  8, 10, },
		},
		.ddr4 = {
			.dq0 = {  1,  2,  3,  0,  6,  4,  5,  7, },
			.dq1 = { 15, 13, 14, 12, 10,  9,  8, 11, },
		},
		.ddr5 = {
			.dq0 = {  1,  0,  3,  2,  6,  7,  4,  5, },
			.dq1 = { 14, 12, 15, 13,  8,  9, 10, 11, },
		},
		.ddr6 = {
			.dq0 = {  0,  2,  1,  3,  4,  7,  5,  6, },
			.dq1 = { 12, 13, 15, 14,  9, 11, 10,  8, },
		},
		.ddr7 = {
			.dq0 = {  3,  2,  1,  0,  5,  4,  6,  7, },
			.dq1 = { 13, 15, 11, 12, 10,  9, 14,  8, },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 }
	},

	.lp5x_config = {
		.ccc_config = 0xff,
	},

	.ect = 1, /* Early Command Training */

};

const struct mb_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * GPIO_MEM_CONFIG_0	GPP_E11
	 * GPIO_MEM_CONFIG_1	GPP_E2
	 * GPIO_MEM_CONFIG_2	GPP_E1
	 * GPIO_MEM_CONFIG_3	GPP_E12
	 */
	gpio_t spd_gpios[] = {
		GPP_E11,
		GPP_E2,
		GPP_E1,
		GPP_E12,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool variant_is_half_populated(void)
{
	/* GPIO_MEM_CH_SEL GPP_E13 */
	return gpio_get(GPP_E13);
}
