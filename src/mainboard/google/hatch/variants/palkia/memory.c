/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <gpio.h>
#include <soc/cnl_memcfg_init.h>
#include <string.h>
#include <variant/gpio.h>

static const struct cnl_mb_cfg baseboard_memcfg = {
	/*
	 * The dqs_map arrays map the SoC pins to the lpddr3 pins
	 * for both channels.
	 *
	 * "The index of the array is CPU byte number, the values are DRAM byte
	 * numbers." - doc #573387
	 *
	 * the index = pin number on SoC
	 * the value = pin number on lpddr3 part
	 */
	.dqs_map[DDR_CH0] = {4, 7, 5, 6, 0, 3, 2, 1},
	.dqs_map[DDR_CH1] = {0, 3, 2, 1, 4, 7, 6, 5},

	.dq_map[DDR_CH0] = {
		{0xf0, 0xf},
		{0x0, 0xf},
		{0xf0, 0xf},
		{0xf0, 0x0},
		{0xff, 0x0},
		{0xff, 0x0}
	},
	.dq_map[DDR_CH1] = {
		{0xf, 0xf0},
		{0x0, 0xf0},
		{0xf, 0xf0},
		{0xf, 0x0},
		{0xff, 0x0},
		{0xff, 0x0}
	},

	/* Palkia uses 200, 80.6 and 162 rcomp resistors */
	.rcomp_resistor = {200, 81, 162},

	/* Palkia Rcomp target values */
	.rcomp_targets = {100, 40, 40, 23, 40},

	/* Set CaVref config to 0 for LPDDR3 */
	.vref_ca_config = 0,

	/* Disable Early Command Training */
	.ect = 0,
};

void variant_memory_params(struct cnl_mb_cfg *bcfg)
{
	memcpy(bcfg, &baseboard_memcfg, sizeof(baseboard_memcfg));
}
