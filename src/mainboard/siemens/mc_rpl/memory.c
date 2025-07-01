/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <soc/romstage.h>

static const struct mb_cfg mc_rpl1_lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  0,  3,  2,  1,  4,  5,  7,  6, },
			.dq1 = { 14, 12, 13, 15,  8, 10,  9, 11, },
		},
		.ddr1 = {
			.dq0 = {  1,  0,  2,  3,  6,  4,  7,  5, },
			.dq1 = { 15, 13, 12, 14,  8, 11,  10,  9, },
		},
		.ddr2 = {
			.dq0 = {  2,  1,  3,  0,  7,  6,  4,  5, },
			.dq1 = { 14, 12, 13, 15,  9, 10, 11,  8, },
		},
		.ddr3 = {
			.dq0 = {  1,  2,  3,  0,  6,  4,  7,  5, },
			.dq1 = { 13, 15, 12, 14,  8, 11, 10,  9, },
		},
		.ddr4 = {
			.dq0 = {  2,  3,  0,  1,  5,  7,  4,  6, },
			.dq1 = { 14, 12, 13, 15, 10,  9, 11,  8, },
		},
		.ddr5 = {
			.dq0 = {  1,  2,  3,  0,  6,  4,  7,  5, },
			.dq1 = { 15, 13, 12, 14, 11,  9, 10,  8, },
		},
		.ddr6 = {
			.dq0 = {  3,  1,  2,  0,  5,  4,  7,  6, },
			.dq1 = {  15, 13, 14, 12, 9, 10,  8, 11, },
		},
		.ddr7 = {
			.dq0 = {  2,  3,  1,  0,  5,  4,  7,  6, },
			.dq1 = { 14, 15, 10, 11, 13,  8,  9, 12, },
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

	.ect = false, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,

	.lp5x_config = {
		.ccc_config = 0xff,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &mc_rpl1_lp5_mem_config;
}
