/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/romstage.h>

static const struct mb_cfg variant_memcfg = {
	.type = MEM_TYPE_LP5X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,
	},

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = { 11, 10,  9,  8, 15, 14, 13, 12 },
			.dq1 = {  7,  5,  4,  6,  1,  2,  0,  3 },
		},
		.ddr1 = {
			.dq0 = {  1,  4,  0,  2,  5,  3,  6,  7 },
			.dq1 = { 14, 13, 12, 15, 11, 10,  9,  8 },
		},
		.ddr2 = {
			.dq0 = { 11, 10,  9,  8, 13, 15, 12, 14 },
			.dq1 = {  4,  6,  7,  5,  0,  1,  2,  3 },
		},
		.ddr3 = {
			.dq0 = {  4,  5,  1,  0,  7,  2,  6,  3 },
			.dq1 = { 15, 11, 10, 14,  9,  8, 13, 12 },
		},
		.ddr4 = {
			.dq0 = { 10,  9,  8, 11, 13, 15, 14, 12 },
			.dq1 = {  5,  4,  3,  6,  2,  1,  0,  7 },
		},
		.ddr5 = {
			.dq0 = {  0,  5,  1,  4,  6,  3,  7,  2 },
			.dq1 = { 10, 11, 13, 15, 14,  9, 12,  8 },
		},
		.ddr6 = {
			.dq0 = {  9, 10, 11,  8, 14, 12, 13, 15 },
			.dq1 = {  5,  7,  6,  4,  1,  2,  3,  0 },
		},
		.ddr7 = {
			.dq0 = {  3,  1,  2,  0,  7,  6,  5,  4 },
			.dq1 = { 10,  9, 15, 13, 11, 12, 14,  8 },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.lp5x_config = {
		.ccc_config = 0xff,
	},

	.LpDdrDqDqsReTraining = 1,

	.ect = 1, /* Early Command Training */

	.UserBd = BOARD_TYPE_ULT_ULX,
};

const struct mb_cfg *variant_memory_params(void)
{
	return &variant_memcfg;
}
