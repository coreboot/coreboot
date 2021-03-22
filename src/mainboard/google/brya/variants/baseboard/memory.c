/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP4X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = {40, 30, 30, 30, 30},
	},

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  0,  1,  2,  3,  4,  5,  6,  7, },
			.dq1 = { 12, 13, 14, 15, 11, 10,  9,  8, },
		},
		.ddr1 = {
			.dq0 = {  7,  2,  6,  3,  5,  1,  4,  0, },
			.dq1 = { 10,  8,  9, 11, 15, 12, 14, 13, },
		},
		.ddr2 = {
			.dq0 = {  3,  2,  1,  0,  4,  5,  6,  7, },
			.dq1 = { 12, 13, 14, 15, 11, 10,  9,  8, },
		},
		.ddr3 = {
			.dq0 = {  7,  0,  1,  6,  5,  4,  2,  3, },
			.dq1 = { 15, 14,  8,  9, 10, 12, 11, 13, },
		},
		.ddr4 = {
			.dq0 = {  3,  2,  1,  0,  4,  5,  6,  7, },
			.dq1 = { 12, 13, 14, 15, 11, 10,  9,  8, },
		},
		.ddr5 = {
			.dq0 = {  3,  4,  2,  5,  0,  6,  1,  7, },
			.dq1 = { 13, 12, 11, 10, 14, 15,  9,  8, },
		},
		.ddr6 = {
			.dq0 = {  3,  2,  1,  0,  7,  4,  5,  6, },
			.dq1 = { 15, 14, 13, 12,  8,  9, 10, 11, },
		},
		.ddr7 = {
			.dq0 = {  3,  4,  2,  5,  1,  0,  7,  6, },
			.dq1 = { 15, 14,  9,  8, 12, 10, 11, 13, },
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
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.ect = 1, /* Enable Early Command Training */
};

const struct mb_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int __weak variant_memory_sku(void)
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

bool __weak variant_is_half_populated(void)
{
	/* GPIO_MEM_CH_SEL GPP_E13 */
	return gpio_get(GPP_E13);
}
