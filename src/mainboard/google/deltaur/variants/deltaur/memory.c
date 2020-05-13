/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <variant/gpio.h>

static const struct lpddr4x_cfg baseboard_memcfg = {
	/* DQ byte map */
	.dq_map = {
		[0] = {
			{ 8,  9, 12,  11, 13, 15,  10, 14, }, /* DDR0_DQ0[7:0] */
			{ 4,  6,  0,   2,  5,  7,   1,  3, }, /* DDR0_DQ1[7:0] */
		},
		[1] = {
			{  2,  3,  0,   6,  1,  7,   5,  4, },  /* DDR1_DQ0[7:0] */
			{ 15, 14, 13,   8, 12, 11,   9, 10, },  /* DDR1_DQ1[7:0] */
		},
		[2] = {
			{  1,  0,  3,   2,  5,  4,   7,  6, }, /* DDR2_DQ0[7:0] */
			{ 14, 15, 12,  13,  8, 10,   9, 11, }, /* DDR2_DQ1[7:0] */
		},
		[3] = {
			{  8, 10, 11,   9, 15, 12,  14, 13, }, /* DDR3_DQ0[7:0] */
			{  4,  7,  6,   5,  2,  0,  1,  3, },  /* DDR3_DQ1[7:0] */
		},
		[4] = {
			{  8,  9, 10,  11, 13, 12,  15, 14, }, /* DDR4_DQ0[7:0] */
			{  7,  6,  4,   5,  0,  2,   1,  3, }, /* DDR4_DQ1[7:0] */
		},
		[5] = {
			{  1,  3,  0,   2,  6,  4,   5,  7, }, /* DDR5_DQ0[7:0] */
			{ 14, 15, 10,  12,  8, 13,  11,  9, }, /* DDR5_DQ1[7:0] */
		},
		[6] = {
			{  1,  0,  2,   4,  5,  3,   7,  6, }, /* DDR6_DQ0[7:0] */
			{ 12, 14, 15,  13,  9,  10,  8, 11, }, /* DDR6_DQ1[7:0] */
		},
		[7] = {
			{ 11,  9,  8,  13, 12,  14, 15, 10, }, /* DDR7_DQ0[7:0] */
			{  4,  7,  5,   1,  2,   6,  3,  0, }, /* DDR7_DQ1[7:0] */
		},
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
		[0] = { 1, 0 }, /* DDR0_DQS[1:0] */
		[1] = { 0, 1 }, /* DDR1_DQS[1:0] */
		[2] = { 0, 1 }, /* DDR2_DQS[1:0] */
		[3] = { 1, 0 }, /* DDR3_DQS[1:0] */
		[4] = { 1, 0 }, /* DDR4_DQS[1:0] */
		[5] = { 0, 1 }, /* DDR5_DQS[1:0] */
		[6] = { 0, 1 }, /* DDR6_DQS[1:0] */
		[7] = { 1, 0 }, /* DDR7_DQS[1:0] */
	},

	.ect = 0, /* Early Command Training */
};

const struct lpddr4x_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

static int variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
		GPIO_MEM_CONFIG_4,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

void variant_memory_init(FSP_M_CONFIG *mem_cfg)
{
	const struct lpddr4x_cfg *board_cfg = variant_memory_params();
	const struct spd_info spd_info = {
		.topology = MEMORY_DOWN,
		.md_spd_loc = SPD_CBFS,
		.cbfs_index = variant_memory_sku(),
	};
	const bool half_populated = false;
	meminit_lpddr4x(mem_cfg, board_cfg, &spd_info, half_populated);
}
