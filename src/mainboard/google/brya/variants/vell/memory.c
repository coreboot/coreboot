/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <console/console.h>
#include <soc/romstage.h>

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
			.dq0 = {  4,  0,  1,  3,  7,  5,  6,  2, },
			.dq1 = {  9, 13, 12,  8, 15, 10, 14, 11, },
		},
		.ddr1 = {
			.dq0 = {  0,  2,  1,  3,  7,  5,  6,  4, },
			.dq1 = { 10,  8, 11,  9, 13, 15, 14, 12, },
		},
		.ddr2 = {
			.dq0 = {  3,  7,  2,  6,  4,  1,  5,  0, },
			.dq1 = { 12, 14, 15, 13, 11,  8, 10,  9, },
		},
		.ddr3 = {
			.dq0 = {  7,  6,  4,  5,  0,  3,  1,  2, },
			.dq1 = {  9, 13,  8, 12, 15, 10, 14, 11, },
		},
		.ddr4 = {
			.dq0 = {  7,  5,  4,  6,  2,  0,  1,  3, },
			.dq1 = { 15, 14, 12, 13, 10,  9,  8, 11, },
		},
		.ddr5 = {
			.dq0 = {  3,  7,  2,  6,  0,  4,  5,  1, },
			.dq1 = {  9, 10, 11,  8, 12, 15, 13, 14, },
		},
		.ddr6 = {
			.dq0 = {  1,  0,  3,  2,  7,  5,  4,  6, },
			.dq1 = { 11,  8, 10,  9, 12, 14, 13, 15, },
		},
		.ddr7 = {
			.dq0 = {  3,  2,  1,  0,  7,  5,  6,  4, },
			.dq1 = {  8,  9, 10, 12, 14, 11, 13, 15, },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.ect = true, /* Enable Early Command Training */

	.lp5x_config = {
		.ccc_config = 0xff,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * GPIO_MEM_CONFIG_0	GPP_E3
	 * GPIO_MEM_CONFIG_1	GPP_E2
	 * GPIO_MEM_CONFIG_2	GPP_E1
	 * GPIO_MEM_CONFIG_3	GPP_E7
	 */
	gpio_t spd_gpios[] = {
		GPP_E3,
		GPP_E2,
		GPP_E1,
		GPP_E7,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool variant_is_half_populated(void)
{
	/* GPIO_MEM_CH_SEL GPP_E5 */
	return gpio_get(GPP_E5);
}
