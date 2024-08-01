/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <soc/romstage.h>

static const struct mb_cfg mu_lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = { 12, 10,  9, 11, 15, 14,  8, 13 },
			.dq1 = {  0,  3,  1,  2,  7,  4,  6,  5 },
		},
		.ddr1 = {
			.dq0 = {  0,  2,  1,  3,  4,  7,  5,  6 },
			.dq1 = { 13, 11,  9, 10,  8, 15, 12, 14 },
		},
		.ddr2 = {
			.dq0 = {  0,  1,  2,  3,  7,  4,  5,  6 },
			.dq1 = {  9, 10,  8, 11, 14, 15, 12, 13 },
		},
		.ddr3 = {
			.dq0 = {  3,  0,  1,  2,  5,  6,  4,  7 },
			.dq1 = { 14, 10,  8, 11, 12, 15,  9, 13 },
		},
		.ddr4 = {
			.dq0 = {  3,  0,  2,  1,  6,  7,  5,  4 },
			.dq1 = { 12, 14, 15, 13,  9, 11,  8, 10 },
		},
		.ddr5 = {
			.dq0 = {  0,  1,  2,  3,  6,  4,  5,  7 },
			.dq1 = { 15, 14, 12, 13,  9, 11,  8, 10 },
		},
		.ddr6 = {
			.dq0 = {  3,  0,  1,  2,  5,  4,  6,  7 },
			.dq1 = { 12, 13, 15, 14,  9, 11, 10,  8 },
		},
		.ddr7 = {
			.dq0 = {  3,  0,  2,  1,  5,  4,  6,  7 },
			.dq1 = { 10,  8, 15, 14,  9, 12, 13, 11 },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 }
	},

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_ULT_ULX,

	.LpDdrDqDqsReTraining = 1,

	.lp5x_config = {
		.ccc_config = 0x00,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &mu_lp5_mem_config;
}
