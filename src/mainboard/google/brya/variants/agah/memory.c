/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP4X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = {40, 30, 30, 30, 30},
	},

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  3,  2,  0,  1,  4,  7,  6,  5, },  /* DDR_A_DQ0 */
			.dq1 = { 12, 13, 14, 15,  9, 10,  8, 11, },  /* DDR_A_DQ1 */
		},
		.ddr1 = {
			.dq0 = { 14,  8,  9, 15, 10, 12, 11, 13, },  /* DDR_A_DQ2 */
			.dq1 = {  1,  7,  6,  0,  5,  3,  4,  2, },  /* DDR_A_DQ3 */
		},
		.ddr2 = {
			.dq0 = {  2,  3,  1,  0,  6,  5,  7,  4, },  /* DDR_A_DQ4 */
			.dq1 = { 12, 13, 14, 15, 10, 11,  9,  8, },  /* DDR_A_DQ5 */
		},
		.ddr3 = {
			.dq0 = {  1,  2,  0,  3,  5,  6,  7,  4, },  /* DDR_A_DQ6 */
			.dq1 = { 15, 14, 13, 12, 10,  9,  8, 11, },  /* DDR_A_DQ7 */
		},
		.ddr4 = {
			.dq0 = {  3,  2,  1,  0,  7,  6,  5,  4, },  /* DDR_B_DQ0 */
			.dq1 = { 12, 15, 13, 14,  8,  9, 10, 11, },  /* DDR_B_DQ1 */
		},
		.ddr5 = {
			.dq0 = { 14,  8,  9, 15, 12, 10, 11, 13, },  /* DDR_B_DQ2 */
			.dq1 = {  1,  7,  6,  0,  5,  2,  4,  3, },  /* DDR_B_DQ3 */
		},
		.ddr6 = {
			.dq0 = { 13, 12, 15, 14,  8, 10,  9, 11, },  /* DDR_B_DQ4 */
			.dq1 = {  7,  4,  6,  5,  1,  0,  3,  2, },  /* DDR_B_DQ5 */
		},
		.ddr7 = {
			.dq0 = {  6,  0,  7,  5,  3,  2,  1,  4, },  /* DDR_B_DQ6 */
			.dq1 = { 10,  8, 13, 12,  9, 14, 15, 11, },  /* DDR_B_DQ7 */
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.ect = 1, /* Enable Early Command Training */
};

const struct mb_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}
