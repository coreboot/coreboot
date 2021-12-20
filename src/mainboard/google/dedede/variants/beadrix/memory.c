/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg board_memcfg_cfg = {

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
	 * the index = pin number on SoC
	 * the value = pin number on LPDDR4 part
	 */
	.dqs_map[DDR_CH0] = {0, 3, 2, 1, 7, 5, 4, 6},
	.dqs_map[DDR_CH1] = {3, 1, 2, 0, 4, 7, 6, 5},

	/* Disable Early Command Training */
	.ect = 1,

	/* User Board Type */
	.UserBd = BOARD_TYPE_MOBILE,
};

const struct mb_cfg *variant_memcfg_config(void)
{
	return &board_memcfg_cfg;
}
