/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>

static const struct mb_cfg board_memcfg = {
	.type = MEM_TYPE_LP4X,

	/* DQ byte map */
	.lp4x_dq_map = {
		.ddr0 = {
			.dq0 = { 3,  2,  1,  0,  4,  5,  7,  6, }, /* DDR0_DQ0[7:0] */
			.dq1 = { 12, 13, 14, 15, 11, 10, 9,  8  }, /* DDR0_DQ1[7:0] */
		},
		.ddr1 = {
			.dq0 = { 0,  7,  1,  6,  2,  5,  3,  4, }, /* DDR1_DQ0[7:0] */
			.dq1 = { 8,  15, 14,  9, 12, 10, 13, 11 }, /* DDR1_DQ1[7:0] */
		},
		.ddr2 = {
			.dq0 = { 2,  3,  0,  1,  4,  5,  6,  7, }, /* DDR2_DQ0[7:0] */
			.dq1 = { 12, 13, 15, 14, 11, 10, 9,  8  }, /* DDR2_DQ1[7:0] */
		},
		.ddr3 = {
			.dq0 = { 7,  0,  1,  6,  5,  4,  2,  3, }, /* DDR3_DQ0[7:0] */
			.dq1 = { 15, 14,  8,  9, 10, 13, 11, 12 }, /* DDR3_DQ1[7:0] */
		},
		.ddr4 = {
			.dq0 = { 4,  5,  2,  3,  7,  6,  0,  1, }, /* DDR4_DQ0[7:0] */
			.dq1 = { 12, 13, 15, 14, 11, 10, 8,  9  }, /* DDR4_DQ1[7:0] */
		},
		.ddr5 = {
			.dq0 = {  3,  4,  2,  5,  0,  6,  1,  7, }, /* DDR5_DQ0[7:0] */
			.dq1 = {  12, 13, 11, 10, 14, 15,  9,  8 }, /* DDR5_DQ1[7:0] */
		},
		.ddr6 = {
			.dq0 = {  3,  2,  1,  0,  7,  4,  5,  6, }, /* DDR6_DQ0[7:0] */
			.dq1 = {  15, 14, 13, 12,  8,  9, 10, 11 }, /* DDR6_DQ1[7:0] */
		},
		.ddr7 = {
			.dq0 = {  2,  4,  3,  5,  1,  0,  7,  6, }, /* DDR7_DQ0[7:0] */
			.dq1 = {  14, 15,  9,  8, 12, 10, 11, 13 }, /* DDR7_DQ1[7:0] */
		},
	},

	/* DQS CPU<>DRAM map */
	.lp4x_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR0_DQS[1:0] */
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR1_DQS[1:0] */
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR2_DQS[1:0] */
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR3_DQS[1:0] */
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR4_DQS[1:0] */
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR5_DQS[1:0] */
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR6_DQS[1:0] */
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR7_DQS[1:0] */
	},

	.ect = true, /* Enable Early Command Training */
};

const struct mb_cfg *variant_memory_params(void)
{
	return &board_memcfg;
}
