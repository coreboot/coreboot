/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <soc/cnl_memcfg_init.h>
#include <string.h>

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
	.dqs_map[DDR_CH0] = {0, 1, 3, 2, 5, 7, 6, 4},
	.dqs_map[DDR_CH1] = {1, 3, 2, 0, 5, 7, 6, 4},

	.dq_map[DDR_CH0] = {
		{0xf, 0xf0},
		{0x0, 0xf0},
		{0xf, 0xf0},
		{0xf, 0x0},
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

	/* Kohaku uses 200, 80.6 and 162 rcomp resistors */
	.rcomp_resistor = {200, 81, 162},

	/* Kohaku Rcomp target values */
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
