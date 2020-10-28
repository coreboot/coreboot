/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include "board_id.h"
#include <baseboard/variants.h>
#include <soc/romstage.h>

static const struct mb_cfg ddr4_mem_config = {
	/* Baseboard uses only 100ohm Rcomp resistors */
	.rcomp_resistor = {100, 100, 100},

	/* Baseboard Rcomp target values */
	.rcomp_targets = {40, 30, 33, 33, 30},

	.dq_pins_interleaved = false,

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,
};

static const struct mb_cfg lpddr4_mem_config = {
	/* DQ byte map */
	.dq_map = {
		{  0,  2,  3,  1,  6,  7,  5,  4, 10,  8, 11,  9, 14, 12, 13, 15 },
		{ 12,  8, 14, 10, 11, 13, 15,  9,  5,  0,  7,  3,  6,  2,  1,  4 },
		{  3,  0,  2,  1,  6,  5,  4,  7, 12, 13, 14, 15, 10,  9,  8, 11 },
		{  2,  6,  7,  1,  3,  4,  0,  5,  9, 13,  8, 15, 14, 11, 12, 10 },
		{  3,  0,  1,  2,  7,  4,  6,  5, 10,  8, 11,  9, 14, 13, 12, 15 },
		{ 10, 12, 14,  8,  9, 13, 15, 11,  3,  7,  6,  2,  0,  4,  5,  1 },
		{ 12, 15, 14, 13,  9, 10, 11,  8,  7,  4,  6,  5,  0,  1,  3,  2 },
		{  0,  2,  4,  3,  1,  6,  7,  5, 13,  9, 10, 11,  8, 12, 14, 15 },
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
		{ 0, 1 }, { 1, 0 }, { 0, 1 }, { 0, 1 }, { 0, 1 }, { 1, 0 }, { 1, 0 }, { 0, 1 }
	},

	.dq_pins_interleaved = false,

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,
};

static const struct mb_cfg lp5_mem_config = {

	/* DQ byte map */
	.dq_map = {
		{  3,  2,  1,  0,  5,  4,  6,  7, 15, 14, 12, 13,  8,  9, 10, 11 },
		{  0,  2,  3,  1,  5,  7,  4,  6, 14, 13, 15, 12,  8,  9, 11, 10 },
		{  1,  2,  0,  3,  4,  6,  5,  7, 15, 13, 12, 14,  9, 10,  8, 11 },
		{  2,  1,  3,  0,  7,  4,  5,  6, 13, 12, 15, 14,  9, 11,  8, 10 },
		{  1,  2,  3,  0,  6,  4,  5,  7, 15, 13, 14, 12, 10,  9,  8, 11 },
		{  1,  0,  3,  2,  6,  7,  4,  5, 14, 12, 15, 13,  8,  9, 10, 11 },
		{  0,  2,  1,  3,  4,  7,  5,  6, 12, 13, 15, 14,  9, 11, 10,  8 },
		{  3,  2,  1,  0,  5,  4,  6,  7, 13, 15, 11, 12, 10,  9, 14,  8 },
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
		{ 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 }
	},

	.dq_pins_interleaved = false,

	.ect = false, /* Early Command Training */

	.lp5_ccc_config = 0xff,

	.UserBd = BOARD_TYPE_MOBILE,
};

static const struct mb_cfg ddr5_mem_config = {
	/* Baseboard uses only 100ohm Rcomp resistors */
	.rcomp_resistor = {100, 100, 100},

	/* Baseboard Rcomp target values */
	.rcomp_targets = {50, 30, 30, 30, 27},

	.dq_pins_interleaved = false,

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,
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
	case ADL_P_DDR5:
		return &ddr5_mem_config;
	case ADL_P_LP5:
		return &lp5_mem_config;
	default:
		die("unsupported board id : 0x%x\n", board_id);
	}
}
