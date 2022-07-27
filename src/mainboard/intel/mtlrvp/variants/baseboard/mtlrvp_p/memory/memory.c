/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/intel/board_id.h>
#include <soc/romstage.h>

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
	default:
		die("Unknown board id = 0x%x\n", board_id);
		break;
	}
}
