/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <gpio.h>
#include <soc/cnl_memcfg_init.h>

static const struct cnl_mb_cfg meowth_lpddr4_cfg = {
	.dq_map[DDR_CH0] = {
		/*
		 * CLK0 goes to package 0 - Bytes[3:0],
		 * CLK1 goes to package 1 - Bytes[7:4]
		 */
		{ 0x0F, 0xF0 },

		/*
		 * Cmd CAA goes to Bytes[3:0],
		 * Cmd CAB goes to Bytes[7:4]
		 */
		{ 0x0F, 0xF0 },

		/* CTL (CS) goes to all bytes */
		{ 0xFF, 0x00 },
	},

	.dq_map[DDR_CH1] = {
		/*
		 * CLK0 goes to package 0 - Bytes[3:0],
		 * CLK1 goes to package 1 - Bytes[7:4]
		 */
		{ 0x0F, 0xF0 },

		/*
		 * Cmd CAA goes to Bytes[3:0],
		 * Cmd CAB goes to Bytes[7:4]
		 */
		{ 0x0F, 0xF0 },

		/* CTL (CS) goes to all bytes */
		{ 0xFF, 0x00 },
	},

	/*
	 * The dqs_map arrays map the lpddr4 pins to the SoC pins
	 * for both channels.
	 *
	 * On Intel's memory spreadsheet, enter the following:
	 *
	 * the index = dqs pin/set number on lpddr4 part
	 * the value = dqs pin/set number on SoC
	 *
	 * and it will translate that and display 8 values per channel.
	 * Those values are copied into the dqs_map arrays below.
	 */
	.dqs_map[DDR_CH0] = { 3, 1, 2, 0, 7, 5, 6, 4 },
	.dqs_map[DDR_CH1] = { 2, 3, 1, 0, 7, 5, 6, 4 },

	/* Meowth uses three 100 Ohm rcomp resistors */
	.rcomp_resistor = { 100, 100, 100 },

	/*
	 * Meowth Rcomp target values.
	 * Rcomp targets for zoombini should be
	 * { 80, 40, 40, 40, 30 }, but we need to
	 * nil out rcomp targets for now to avoid bug b:70896346
	 */
	.rcomp_targets = { 0, 0, 0, 0, 0 },

	/* Meowth is a non-interleaved design */
	.dq_pins_interleaved = 0,

	/* Enable Early Command Training */
	.ect = 1,
};

const struct cnl_mb_cfg *variant_lpddr4_config(void)
{
	return &meowth_lpddr4_cfg;
}
