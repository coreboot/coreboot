/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>

static const struct mb_cfg board_memcfg = {
	.type = MEM_TYPE_LP4X,

	/* DQ byte map */
	.lp4x_dq_map = {
		.ddr0 = {
			.dq0 = { 7,  3,  1,  4,  0,  5,  2,  6, }, /* DDR0_DQ0[7:0] */
			.dq1 = { 13, 14, 8,  10, 9,  15, 11, 12 }, /* DDR0_DQ1[7:0] */
		},
		.ddr1 = {
			.dq0 = { 1,  2,  7,  6,  3,  5,  4,  0, }, /* DDR1_DQ0[7:0] */
			.dq1 = { 14, 15, 13, 10,  8, 11, 12, 9  }, /* DDR1_DQ1[7:0] */
		},
		.ddr2 = {
			.dq0 = { 11, 15, 10, 9,  8, 12, 13, 14, }, /* DDR2_DQ0[7:0] */
			.dq1 = { 5,  6,  4,  0,  7,  2,  3,  1  }, /* DDR2_DQ1[7:0] */
		},
		.ddr3 = {
			.dq0 = { 11, 15, 10, 9, 13, 12, 14,  8, }, /* DDR3_DQ0[7:0] */
			.dq1 = { 0,  5,  6,  4,  1,  2,  7,  3  }, /* DDR3_DQ1[7:0] */
		},
		.ddr4 = {
			.dq0 = { 7,  2,  3,  1,  4,  0,  5,  6, }, /* DDR4_DQ0[7:0] */
			.dq1 = { 13, 14,  8, 12, 10,  9, 15, 11 }, /* DDR4_DQ1[7:0] */
		},
		.ddr5 = {
			.dq0 = { 7,  3,  2,  1,  6,  4,  0,  5, }, /* DDR5_DQ0[7:0] */
			.dq1 = { 15, 14, 12, 8, 11, 13,  9,  10 }, /* DDR5_DQ1[7:0] */
		},
		.ddr6 = {
			.dq0 = { 11, 10, 15, 12, 8,  9, 14, 13, }, /* DDR6_DQ0[7:0] */
			.dq1 = { 6,  0,  5,  4,  3,  2,  7,  1  }, /* DDR6_DQ1[7:0] */
		},
		.ddr7 = {
			.dq0 = { 9, 10, 11,  8, 12, 14, 13, 15, }, /* DDR7_DQ0[7:0] */
			.dq1 = { 0,  5,  4,  7,  1,  6,  3,  2  }, /* DDR7_DQ1[7:0] */
		},
	},

	/* DQS CPU<>DRAM map */
	.lp4x_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR0_DQS[1:0] */
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR1_DQS[1:0] */
		.ddr2 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR2_DQS[1:0] */
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR3_DQS[1:0] */
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR4_DQS[1:0] */
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },  /* DDR5_DQS[1:0] */
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR6_DQS[1:0] */
		.ddr7 = { .dqs0 = 1, .dqs1 = 0 },  /* DDR7_DQS[1:0] */
	},

	.ect = true, /* Enable Early Command Training */
};

const struct mb_cfg *variant_memory_params(void)
{
	return &board_memcfg;
}
