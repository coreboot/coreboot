/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>

static const struct mb_cfg board_memcfg = {
	.type = MEM_TYPE_LP4X,

	/* DQ byte map */
	.lp4x_dq_map = {
		.ddr0 = {
			.dq0 = { 10, 12, 13,  9, 11,  8, 15, 14,}, /* DDR0_DQ0[7:0] */
			.dq1 = { 3,  0,  1,  5,  4,  7,  6,  2  }, /* DDR0_DQ1[7:0] */
		},
		.ddr1 = {
			.dq0 = { 8, 10, 13,  9, 12, 15, 11, 14, }, /* DDR1_DQ0[7:0] */
			.dq1 = { 3,  5,  7,  2,  1,  0,  4,  6  }, /* DDR1_DQ1[7:0] */
		},
		.ddr2 = {
			.dq0 = { 1,  3,  0,  2,  5,  4,  7,  6, }, /* DDR2_DQ0[7:0] */
			.dq1 = { 15, 14, 12, 13, 8,  9, 10, 11  }, /* DDR2_DQ1[7:0] */
		},
		.ddr3 = {
			.dq0 = { 8,  9, 10, 11, 14, 12, 15, 13, }, /* DDR3_DQ0[7:0] */
			.dq1 = { 5,  6,  7,  4,  2,  3,  1,  0  }, /* DDR3_DQ1[7:0] */
		},
		.ddr4 = {
			.dq0 = { 9,  8, 10, 11, 12, 13, 14, 15, }, /* DDR4_DQ0[7:0] */
			.dq1 = { 6,  7,  4,  5,  0,  2,  1,  3  }, /* DDR4_DQ1[7:0] */
		},
		.ddr5 = {
			.dq0 = { 0,  1,  3,  2,  7,  4,  5,  6, }, /* DDR5_DQ0[7:0] */
			.dq1 = { 15, 14,  9, 12,  8, 13, 11, 10 }, /* DDR5_DQ1[7:0] */
		},
		.ddr6 = {
			.dq0 = { 7,  5,  3,  6,  1,  0,  4,  2, }, /* DDR6_DQ0[7:0] */
			.dq1 = { 12, 14, 15, 13,  8, 11,  9, 10 }, /* DDR6_DQ1[7:0] */
		},
		.ddr7 = {
			.dq0 = { 3,  7,  1,  6,  5,  4,  2,  0, }, /* DDR7_DQ0[7:0] */
			.dq1 = { 12, 11,  8, 14, 10,  9, 15, 13 }, /* DDR7_DQ1[7:0] */
		},
	},

	/* DQS CPU<>DRAM map */
	.lp4x_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR0_DQS[1:0] */
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR1_DQS[1:0] */
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR2_DQS[1:0] */
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR3_DQS[1:0] */
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR4_DQS[1:0] */
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
