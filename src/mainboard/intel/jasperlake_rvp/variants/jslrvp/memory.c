/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg jslrvp_ddr4_memcfg_cfg = {

	.dq_map[DDR_CH0] = {
		{0xf, 0xf0},
		{0xf, 0xf0},
		{0xff, 0x0},
		{0x0, 0x0},
		{0x0, 0x0},
		{0x0, 0x0}
	},

	.dq_map[DDR_CH1] = {
		{0xf, 0xf0},
		{0xf, 0xf0},
		{0xff, 0x0},
		{0x0, 0x0},
		{0x00, 0x0},
		{0x00, 0x0}
	},

	/*
	 * The dqs_map arrays map the ddr4 pins to the SoC pins
	 * for both channels.
	 *
	 * the index = pin number on ddr4 part
	 * the value = pin number on SoC
	 */
	.dqs_map[DDR_CH0] = {0, 1, 3, 2, 4, 5, 6, 7},
	.dqs_map[DDR_CH1] = {1, 0, 4, 5, 2, 3, 6, 7},

	/* Baseboard uses 100, 100 and 100 rcomp resistors */
	.rcomp_resistor = {100, 100, 100},

	/* Baseboard Rcomp target values */
	.rcomp_targets = {0, 0, 0, 0, 0},

	/* Enable Early Command Training */
	.ect = 1,

	/* Set Board Type */
	.UserBd = BOARD_TYPE_MOBILE,
};

static const struct mb_cfg jslrvp_lpddr4_memcfg_cfg = {

	.dq_map[DDR_CH0] = {
		{0xf, 0xf0},
		{0xf, 0xf0},
		{0xff, 0x0},
		{0x0, 0x0},
		{0x0, 0x0},
		{0x0, 0x0}
	},

	.dq_map[DDR_CH1] = {
		{0xf, 0xf0},
		{0xf, 0xf0},
		{0xff, 0x0},
		{0x0, 0x0},
		{0x00, 0x0},
		{0x00, 0x0}
	},

	/*
	 * The dqs_map arrays map the ddr4 pins to the SoC pins
	 * for both channels.
	 *
	 * the index = pin number on ddr4 part
	 * the value = pin number on SoC
	 */
	.dqs_map[DDR_CH0] = {0, 3, 2, 1, 7, 5, 4, 6},
	.dqs_map[DDR_CH1] = {3, 1, 2, 0, 4, 7, 6, 5},

	/* Baseboard uses 100, 100 and 100 rcomp resistors */
	.rcomp_resistor = {100, 100, 100},

	/*
	 * Baseboard Rcomp target values.
	 */
	.rcomp_targets = {80, 40, 40, 40, 30},

	/* Enable Early Command Training */
	.ect = 1,

	/* Set Board Type */
	.UserBd = BOARD_TYPE_ULT_ULX,
};

const struct mb_cfg *variant_memcfg_config(uint8_t board_id)
{
	if (board_id == jsl_ddr4)
		return &jslrvp_ddr4_memcfg_cfg;
	else if (board_id == jsl_lpddr4)
		return &jslrvp_lpddr4_memcfg_cfg;

	die("unsupported board id : 0x%x\n", board_id);
}
