/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/intel/board_id.h>
#include <soc/romstage.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* DQ byte map as per doc #573387 */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  0,  3,  1,  2,  4,  5,  6,  7, },
			.dq1 = {  12,  13,  14,  15,  11,  9,  10,  8 },
		},
		.ddr1 = {
			.dq0 = {  3,  0,  1,  2,  5,  7,  6,  4, },
			.dq1 = {  13,  10,  12,  15,  9,  11,  8,  14 },
		},
		.ddr2 = {
			.dq0 = {  2,  1,  3,  0,  7,  5,  6,  4, },
			.dq1 = {  12,  8,  13,  15,  11,  10,  9,  14 },
		},
		.ddr3 = {
			.dq0 = {  4,  3,  0,  1,  5,  2,  6,  7, },
			.dq1 = {  8,  15,  12,  14,  10,  13,  9,  11 },
		},
		.ddr4 = {
			.dq0 = {  1,  3,  2,  6,  7,  5,  4,  0, },
			.dq1 = {  14,  13,  12,  15,  11,  10,  8,  9 },
		},
		.ddr5 = {
			.dq0 = {  0,  7,  3,  6,  2,  5,  1,  4, },
			.dq1 = {  9,  10,  11,  8,  13,  14,  15,  12 },
		},
		.ddr6 = {
			.dq0 = {  3,  0,  2,  1,  6,  5,  4,  7, },
			.dq1 = {  15,  13,  12,  14,  8,  11,  10,  9 },
		},
		.ddr7 = {
			.dq0 = {  2,  1,  3,  0,  6,  4,  7,  5, },
			.dq1 = {  14,  12,  13,  15,  9,  10,  11,  8 },
		},
	},

	/* DQS CPU<>DRAM map as per doc #573387 */
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

	.LpDdrDqDqsReTraining = 1,

	.UserBd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0,
	},
};

static const struct mb_cfg ddr5_mem_config = {
	.type = MEM_TYPE_DDR5,

	.rcomp = {
		/* As per doc #729782, baseboard uses only 100 Ohm Rcomp resistor */
		.resistor = 100,
	},

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_ULT_ULX,

	.LpDdrDqDqsReTraining = 1,

	.ddr_config = {
		.dq_pins_interleaved = false,
	}
};

const struct mb_cfg *variant_memory_params(void)
{
	int board_id = get_rvp_board_id();

	switch (board_id) {
	case MTLP_DDR5_RVP:
		return &ddr5_mem_config;
	case MTLP_LP5_T3_RVP:
	case MTLP_LP5_T4_RVP:
		return &lp5_mem_config;
	default:
		die("Unknown board id = 0x%x\n", board_id);
		break;
	}
}
