/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg mc_ehl_lpddr4x_memcfg_cfg = {

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
		{0x0, 0x0},
		{0x0, 0x0}
	},

	/*
	 * The dqs_map arrays map the ddr4 pins to the SoC pins
	 * for both channels.
	 *
	 * the index = pin number on ddr4 part
	 * the value = pin number on SoC
	 */
	.dqs_map[DDR_CH0] = {3, 0, 1, 2, 7, 4, 5, 6},
	.dqs_map[DDR_CH1] = {3, 0, 1, 2, 7, 4, 5, 6},

	/* Baseboard uses 100, 100 and 100 rcomp resistors */
	.rcomp_resistor = {100, 100, 100},

	.rcomp_targets = {60, 40, 30, 20, 30},

	/* LPDDR4x does not allow interleaved memory */
	.dq_pins_interleaved = 0,

	/* Baseboard is using config 2 for vref_ca */
	.vref_ca_config = 2,

	/* Enable Early Command Training */
	.ect = 1,

	/* Set Board Type */
	.UserBd = BOARD_TYPE_MOBILE,
};

const struct mb_cfg *variant_memcfg_config(void)
{
	return &mc_ehl_lpddr4x_memcfg_cfg;
}
