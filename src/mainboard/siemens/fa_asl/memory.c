/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <soc/romstage.h>

static const struct mb_cfg fa_asl_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* DQ byte map. Since Amston Lake is based on Alder Lake N, it only supports
	  64 bit for memory. */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = { 12,  9, 10, 11, 14, 13,  8, 15 },
			.dq1 = {  3,  1,  2,  0,  4,  7,  5,  6 },
		},
		.ddr1 = {
			.dq0 = {  3,  1,  2,  0,  4,  7,  5,  6 },
			.dq1 = { 13,  9,  8, 11, 10, 14, 15, 12 },
		},
		.ddr2 = {
			.dq0 = {  2,  1,  3,  0,  4,  6,  5,  7 },
			.dq1 = {  8,  9, 10, 11, 13, 14, 12, 15 },
		},
		.ddr3 = {
			.dq0 = {  3,  0,  1,  2,  5,  6,  4,  7 },
			.dq1 = { 13,  9, 11,  8, 14, 15, 10, 12 },
		}
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 }
	},

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_ULT_ULX,

	.LpDdrDqDqsReTraining = 1,

	.lp5x_config = {
		.ccc_config = 0xff,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &fa_asl_mem_config;
}
