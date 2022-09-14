/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <soc/romstage.h>

#include "board_id.h"

static const struct mb_cfg ddr4_mem_config = {
	.type = MEM_TYPE_DDR4,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistor */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = { 50, 20, 25, 25, 25 },
	},

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,

	.LpDdrDqDqsReTraining = 1,

	.ddr_config = {
		.dq_pins_interleaved = false,
	},
};

static const struct mb_cfg lpddr4_mem_config = {
	.type = MEM_TYPE_LP4X,

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  0,  2,  3,  1,  6,  7,  5,  4, },
			.dq1 = { 10,  8, 11,  9, 14, 12, 13, 15, },
		},
		.ddr1 = {
			.dq0 = { 12,  8, 14, 10, 11, 13, 15,  9, },
			.dq1 = {  5,  0,  7,  3,  6,  2,  1,  4, },
		},
		.ddr2 = {
			.dq0 = {  3,  0,  2,  1,  6,  5,  4,  7, },
			.dq1 = { 12, 13, 14, 15, 10,  9,  8, 11, },
		},
		.ddr3 = {
			.dq0 = {  2,  6,  7,  1,  3,  4,  0,  5, },
			.dq1 = {  9, 13,  8, 15, 14, 11, 12, 10, },
		},
		.ddr4 = {
			.dq0 = {  3,  0,  1,  2,  7,  4,  6,  5, },
			.dq1 = { 10,  8, 11,  9, 14, 13, 12, 15, },
		},
		.ddr5 = {
			.dq0 = { 10, 12, 14,  8,  9, 13, 15, 11, },
			.dq1 = {  3,  7,  6,  2,  0,  4,  5,  1, },
		},
		.ddr6 = {
			.dq0 = { 12, 15, 14, 13,  9, 10, 11,  8, },
			.dq1 = {  7,  4,  6,  5,  0,  1,  3,  2, },
		},
		.ddr7 = {
			.dq0 = {  0,  2,  4,  3,  1,  6,  7,  5, },
			.dq1 = { 13,  9, 10, 11,  8, 12, 14, 15, },
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

	.LpDdrDqDqsReTraining = 1,

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,
};

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  3,  2,  1,  0,  5,  4,  6,  7, },
			.dq1 = { 15, 14, 12, 13,  8,  9, 10, 11, },
		},
		.ddr1 = {
			.dq0 = {  0,  2,  3,  1,  5,  7,  4,  6, },
			.dq1 = { 14, 13, 15, 12,  8,  9, 11, 10, },
		},
		.ddr2 = {
			.dq0 = {  1,  2,  0,  3,  4,  6,  5,  7, },
			.dq1 = { 15, 13, 12, 14,  9, 10,  8, 11, },
		},
		.ddr3 = {
			.dq0 = {  2,  1,  3,  0,  7,  4,  5,  6, },
			.dq1 = { 13, 12, 15, 14,  9, 11,  8, 10, },
		},
		.ddr4 = {
			.dq0 = {  1,  2,  3,  0,  6,  4,  5,  7, },
			.dq1 = { 15, 13, 14, 12, 10,  9,  8, 11, },
		},
		.ddr5 = {
			.dq0 = {  1,  0,  3,  2,  6,  7,  4,  5, },
			.dq1 = { 14, 12, 15, 13,  8,  9, 10, 11, },
		},
		.ddr6 = {
			.dq0 = {  0,  2,  1,  3,  4,  7,  5,  6, },
			.dq1 = { 12, 13, 15, 14,  9, 11, 10,  8, },
		},
		.ddr7 = {
			.dq0 = {  3,  2,  1,  0,  5,  4,  6,  7, },
			.dq1 = { 13, 15, 11, 12, 10,  9, 14,  8, },
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

	.ect = true, /* Early Command Training */

	.LpDdrDqDqsReTraining = 1,

	.UserBd = BOARD_TYPE_MOBILE,

	.lp5x_config = {
		.ccc_config = 0xff,
	},
};

static const struct mb_cfg ddr5_mem_config = {
	.type = MEM_TYPE_DDR5,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistor */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = { 50, 30, 30, 30, 27 },
	},

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,

	.LpDdrDqDqsReTraining = 1,

	.ddr_config = {
		.dq_pins_interleaved = false,
	}
};

static const struct mb_cfg adlm_lp4_mem_config = {
	.type = MEM_TYPE_LP4X,

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = { 13, 12, 14, 8, 11, 10, 9, 15, }, /* DDR0_DQ0[7:0] */
			.dq1 = {  3,  2,  7, 6,  0,  1, 5,  4, }, /* DDR0_DQ1[7:0] */
		},
		.ddr1 = {
			.dq0 = { 11, 15, 10, 9, 12, 8, 14, 13, },  /* DDR1_DQ0[7:0] */
			.dq1 = {  0,  1,  7, 6,  2, 5,  4,  3, },  /* DDR1_DQ1[7:0] */
		},
		.ddr2 = {
			.dq0 = {  6, 7,  3,  2,  0, 4,  1,  5, }, /* DDR2_DQ0[7:0] */
			.dq1 = { 14, 8, 13, 12, 11, 9, 10, 15, }, /* DDR2_DQ1[7:0] */
		},
		.ddr3 = {
			.dq0 = { 2,  6,  7,  3,  1,  5, 0,  4, }, /* DDR3_DQ0[7:0] */
			.dq1 = { 8, 14, 13, 12, 10, 11, 9, 15, }, /* DDR3_DQ1[7:0] */
		},
		.ddr4 = {
			.dq0 = { 8, 14, 13, 12, 10, 11, 9, 15, }, /* DDR3_DQ1[7:0] */
			.dq1 = { 1,  0,  5,  4,  6,  2, 3,  7, }, /* DDR4_DQ1[7:0] */
		},
		.ddr5 = {
			.dq0 = { 8, 10, 9, 12, 14, 11, 13, 15, }, /* DDR5_DQ0[7:0] */
			.dq1 = { 0,  7, 2,  6,  3,  1,  4,  5, }, /* DDR5_DQ1[7:0] */
		},
		.ddr6 = {
			.dq0 = { 14, 12, 9, 8, 15, 10, 13, 11, }, /* DDR6_DQ0[7:0] */
			.dq1 = {  4,  0, 5, 6,  3,  2,  1,  7, }, /* DDR6_DQ1[7:0] */
		},
		.ddr7 = {
			.dq0 = { 10, 15, 12, 11, 9, 14, 13, 8, }, /* DDR7_DQ0[7:0] */
			.dq1 = {  7,  1,  2,  3, 6,  0,  5, 4, }, /* DDR7_DQ1[7:0] */
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 1, .dqs1 = 0 }
	},

	.ect = true, /* Early Command Training */

	.CmdMirror = 0xCC,

	.LpDdrDqDqsReTraining = 1,

	.UserBd = BOARD_TYPE_ULT_ULX_T4,
};

static const struct mb_cfg adlm_lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  4,  5,  7,  6,  3,  2,  1,  0, },
			.dq1 = { 12, 10,  8, 15, 11,  9, 14, 13, },
		},
		.ddr1 = {
			.dq0 = {  1,  0,  2,  3,  7,  4,  5,  6, },
			.dq1 = { 14, 15, 10, 11, 13, 12,  8,  9, },
		},
		.ddr2 = {
			.dq0 = {  7,  4,  2,  0,  3,  1,  6,  5, },
			.dq1 = { 14, 13, 15, 12,  8,  9, 10, 11, },
		},
		.ddr3 = {
			.dq0 = {  3,  2,  0,  1,  7,  5,  6,  4, },
			.dq1 = { 12, 14, 15, 13, 11,  8, 10,  9, },
		},
		.ddr4 = {
			.dq0 = {  2,  3,  0,  1,  6,  4,  7,  5, },
			.dq1 = { 14,  9, 11, 13, 12,  8, 15, 10, },
		},
		.ddr5 = {
			.dq0 = {  4,  7,  3,  1,  5,  2,  6,  0, },
			.dq1 = { 14,  8, 11,  9, 12, 15, 10, 13, },
		},
		.ddr6 = {
			.dq0 = { 10, 11, 13,  9, 15, 12,  8, 14, },
			.dq1 = {  2,  4,  7,  0,  6,  3,  5,  1, },
		},
		.ddr7 = {
			.dq0 = { 13, 15, 11, 14, 10, 12,  8,  9, },
			.dq1 = {  6,  5,  4,  7,  3,  1,  2,  0, },
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
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 1, .dqs1 = 0 }
	},

	.ect = false, /* Early Command Training */

	.UserBd = BOARD_TYPE_ULT_ULX_T4,

	.lp5x_config = {
		.ccc_config = 0xff,
	},
};

static const struct mb_cfg adln_lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* DQ byte map */
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
		},
		.ddr4 = {
			.dq0 = { 12,  9, 10, 11, 14, 13,  8, 15 },
			.dq1 = {  3,  1,  2,  0,  4,  7,  5,  6 },
		},
		.ddr5 = {
			.dq0 = {  3,  1,  2,  0,  4,  7,  5,  6 },
			.dq1 = { 13,  9,  8, 11, 10, 14, 15, 12 },
		},
		.ddr6 = {
			.dq0 = {  2,  1,  3,  0,  4,  6,  5,  7 },
			.dq1 = {  8,  9, 10, 11, 13, 14, 12, 15 },
		},
		.ddr7 = {
			.dq0 = {  3,  0,  1,  2,  5,  6,  4,  7 },
			.dq1 = { 13,  9, 11,  8, 14, 15, 10, 12 },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 }
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
	int board_id = get_board_id();

	switch (board_id) {
	case ADL_P_LP4_1:
	case ADL_P_LP4_2:
		return &lpddr4_mem_config;
	case ADL_P_DDR4_1:
	case ADL_P_DDR4_2:
		return &ddr4_mem_config;
	case ADL_P_DDR5_1:
	case ADL_P_DDR5_2:
		return &ddr5_mem_config;
	case ADL_P_LP5_1:
	case ADL_P_LP5_2:
		return &lp5_mem_config;
	case ADL_M_LP4:
		return &adlm_lp4_mem_config;
	case ADL_M_LP5:
		return &adlm_lp5_mem_config;
	case ADL_N_LP5:
		return &adln_lp5_mem_config;
	default:
		die("unsupported board id : 0x%x\n", board_id);
	}
}
