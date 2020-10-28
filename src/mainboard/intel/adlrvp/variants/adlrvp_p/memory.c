/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include "../../board_id.h"
#include <baseboard/variants.h>
#include <soc/romstage.h>

static const struct mb_cfg ddr4_mem_config = {
	/* Baseboard uses only 100ohm Rcomp resistors */
	.rcomp_resistor = {100, 100, 100},

	/* Baseboard Rcomp target values */
	.rcomp_targets = {40, 30, 33, 33, 30},

	.dq_pins_interleaved = true,

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,
};

static const struct mb_cfg lpddr4_mem_config = {
	/* DQ byte map */
	.dq_map = {
	{   0,  2,   3,  1,   6,   7,   5,   4,    /* Byte 0 */
	   10,  8,  11,  9,  14,  12,  13,  15 },  /* Byte 1 */
	{  12,  8,  14, 10,  11,  13,  15,   9,    /* Byte 2 */
	    5,  0,   7,  3,   6,   2,   1,   4 },  /* Byte 3 */
	{   3,  0,   2,  1,   6,   5,   4,   7,    /* Byte 4 */
	   12, 13,  14, 15,  10,   9,   8,  11 },  /* Byte 5 */
	{   2,  6,   7,  1,   3,   4,   0,   5,    /* Byte 6 */
	    9, 13,   8, 15,  14,  11,  12,  10 },  /* Byte 7 */
	{   3,  0,   1,  2,   7,   4,   6,   5,    /* Byte 0 */
	   10,  8,  11,  9,  14,  13,  12,  15 },  /* Byte 1 */
	{  10, 12,  14,  8,   9,  13,  15,  11,    /* Byte 2 */
	    3,  7,   6,  2,   0,   4,   5,  1 },   /* Byte 3 */
	{  12, 15,  14, 13,   9,  10,  11,  8,     /* Byte 4 */
	    7,  4,   6,  5,   0,   1,   3,  2 },   /* Byte 5 */
	{   0,  2,   4,  3,   1,   6,   7,  5,     /* Byte 6 */
	   13,  9,  10, 11,   8,  12,  14, 15 },   /* Byte 7 */
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
	/* Ch 0     1         2         3  */
	{ 0, 1 }, { 1, 0 }, { 0, 1 }, { 0, 1 },
	{ 0, 1 }, { 1, 0 }, { 1, 0 }, { 0, 1 }
	},

	.dq_pins_interleaved = false,

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,
};

const struct mb_cfg *variant_memory_params(void)
{
	int board_id = get_board_id();

	if (board_id == ADL_P_LP4_1 || board_id == ADL_P_LP4_2)
		return &lpddr4_mem_config;
	else if (board_id == ADL_P_DDR4_1 || board_id == ADL_P_DDR4_2)
		return &ddr4_mem_config;

	die("unsupported board id : 0x%x\n", board_id);
}
