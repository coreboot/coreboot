/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/romstage.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* DQ CPU<>DRAM map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = { 10,  8,  9, 12, 15, 13, 14, 11, },	/* DDR0_DQ0[7:0] */
			.dq1 = {  2,  6,  3,  7,  5,  1,  4,  0, },	/* DDR0_DQ1[7:0] */
		},
		.ddr1 = {
			.dq0 = {  2,  0,  3,  1,  6,  4,  7,  5, },	/* DDR1_DQ0[7:0] */
			.dq1 = {  8,  9, 10, 11, 13, 12, 14, 15, },	/* DDR1_DQ1[7:0] */
		},
		.ddr2 = {
			.dq0 = {  1,  0,  3,  2,  6,  4,  5,  7, },	/* DDR2_DQ0[7:0] */
			.dq1 = { 12, 13,  8,  9, 15, 11, 14, 10, },	/* DDR2_DQ1[7:0] */
		},
		.ddr3 = {
			.dq0 = {  8,  9, 11, 10, 13, 15, 14, 12, },	/* DDR3_DQ0[7:0] */
			.dq1 = {  6,  5,  4,  7,  3,  2,  0,  1, },	/* DDR3_DQ1[7:0] */
		},
		.ddr4 = {
			.dq0 = {  8, 13,  9, 12, 15, 11, 14, 10, },	/* DDR4_DQ0[7:0] */
			.dq1 = {  2,  7,  3,  6,  5,  1,  4,  0, },	/* DDR4_DQ1[7:0] */
		},
		.ddr5 = {
			.dq0 = {  0,  2,  1,  3,  6,  7,  4,  5, },	/* DDR5_DQ0[7:0] */
			.dq1 = { 13, 12, 15, 14, 10,  9,  8, 11, },	/* DDR5_DQ1[7:0] */
		},
		.ddr6 = {
			.dq0 = {  8, 13,  9, 12, 15, 10, 14, 11, },	/* DDR6_DQ0[7:0] */
			.dq1 = {  3,  6,  2,  7,  4,  1,  0,  5, },	/* DDR6_DQ1[7:0] */
		},
		.ddr7 = {
			.dq0 = { 11,  9, 10,  8, 12, 14, 13, 15, },	/* DDR7_DQ0[7:0] */
			.dq1 = {  4,  6,  1,  0,  7,  3,  2,  5, }	/* DDR7_DQ1[7:0] */
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR0_DQS[1:0] */
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR1_DQS[1:0] */
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR2_DQS[1:0] */
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR3_DQS[1:0] */
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR4_DQS[1:0] */
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR5_DQS[1:0] */
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR6_DQS[1:0] */
		.ddr7 = { .dqs0 = 1, .dqs1 = 0 }   /* DDR7_DQS[1:0] */
	},

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,

	.lp5x_config = {
		.ccc_config = 0xD0,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &lp5_mem_config;
}

int variant_memory_sku(void)
{
	const gpio_t spd_gpios[] = {
		GPP_A7,
		GPP_A20,
		GPP_A19,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}
