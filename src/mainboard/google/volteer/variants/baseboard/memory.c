/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct lpddr4x_cfg baseboard_memcfg = {
	/* DQ CPU<>DRAM map */
	.dq_map = {
		[0] = {
			{ 0,  1,  2,  3,  4,  5,  6,  7, },  /* DDR0_DQ0[7:0] */
			{ 12, 13, 14, 15, 11, 10,  9,  8, }, /* DDR0_DQ1[7:0] */
		},
		[1] = {
			{ 7,  2,  6,  3,  5,  1,  4,  0, },  /* DDR1_DQ0[7:0] */
			{ 10,  8,  9, 11, 15, 12, 14, 13, }, /* DDR1_DQ1[7:0] */
		},
		[2] = {
			{ 3,  2,  1,  0,  4,  5,  6,  7, },  /* DDR2_DQ0[7:0] */
			{ 12, 13, 14, 15, 11, 10,  9,  8, }, /* DDR2_DQ1[7:0] */
		},
		[3] = {
			{ 7,  0,  1,  6,  5,  4,  2,  3, },  /* DDR3_DQ0[7:0] */
			{ 15, 14,  8,  9, 10, 12, 11, 13, }, /* DDR3_DQ1[7:0] */
		},
		[4] = {
			{ 3,  2,  1,  0,  4,  5,  6,  7, },  /* DDR4_DQ0[7:0] */
			{ 12, 13, 14, 15, 11, 10,  9,  8, }, /* DDR4_DQ1[7:0] */
		},
		[5] = {
			{ 3,  4,  2,  5,  0,  6,  1,  7, },  /* DDR5_DQ0[7:0] */
			{ 13, 12, 11, 10, 14, 15,  9,  8, }, /* DDR5_DQ1[7:0] */
		},
		[6] = {
			{ 3,  2,  1,  0,  7,  4,  5,  6, },  /* DDR6_DQ0[7:0] */
			{ 15, 14, 13, 12,  8,  9, 10, 11, }, /* DDR6_DQ1[7:0] */
		},
		[7] = {
			{ 3,  4,  2,  5,  1,  0,  7,  6, },  /* DDR7_DQ0[7:0] */
			{ 15, 14,  9,  8, 12, 10, 11, 13, }, /* DDR7_DQ1[7:0] */
		},
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
		[0] = { 0, 1 },  /* DDR0_DQS[1:0] */
		[1] = { 0, 1 },  /* DDR1_DQS[1:0] */
		[2] = { 0, 1 },  /* DDR2_DQS[1:0] */
		[3] = { 0, 1 },  /* DDR3_DQS[1:0] */
		[4] = { 0, 1 },  /* DDR4_DQS[1:0] */
		[5] = { 0, 1 },  /* DDR5_DQS[1:0] */
		[6] = { 0, 1 },  /* DDR6_DQS[1:0] */
		[7] = { 0, 1 },  /* DDR7_DQS[1:0] */
	},

	.ect = 1, /* Enable Early Command Training */
};

const struct lpddr4x_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int __weak variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}
