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
			.dq0 = {  0,  3,  1,  2,  7, 6,  4, 5, },
			.dq1 = { 13, 12, 14, 15, 11, 8, 10, 9, },
		},
		.ddr1 = {
			.dq0 = { 7, 6,  5,  4,  2,  1,  0,  3, },
			.dq1 = { 9, 8, 10, 11, 13, 14, 15, 12, },
		},
		.ddr2 = {
			.dq0 = { 8, 11, 9, 10, 12, 14, 13, 15, },
			.dq1 = { 5,  7, 6,  4,  1,  2,  3,  0, },
		},
		.ddr3 = {
			.dq0 = {  2,  0,  1,  3,  7, 6, 5,  4, },
			.dq1 = { 12, 13, 14, 15, 11, 8, 9, 10, },
		},
		.ddr4 = {
			.dq0 = {  0,  3,  1,  2,  7, 5,  6, 4, },
			.dq1 = { 12, 14, 13, 15, 10, 8, 11, 9, },
		},
		.ddr5 = {
			.dq0 = { 10, 8, 9, 11, 13, 15, 14, 12, },
			.dq1 = {  7, 6, 5,  4,  3,  1,  0,  2, },
		},
		.ddr6 = {
			.dq0 = { 6, 4,  5,  7,  1,  0,  2,  3, },
			.dq1 = { 8, 9, 10, 11, 13, 15, 14, 12, },
		},
		.ddr7 = {
			.dq0 = {  1,  2,  0,  3,  5, 6,  7, 4, },
			.dq1 = { 12, 13, 14, 15, 11, 8, 10, 9, },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
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
