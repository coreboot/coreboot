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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <compiler.h>
#include <gpio.h>
#include <soc/cnl_lpddr4_init.h>

static const struct lpddr4_cfg baseboard_lpddr4_cfg = {
	.dq_map[LP4_CH0] = {
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

	.dq_map[LP4_CH1] = {
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
	 * the index = pin number on lpddr4 part
	 * the value = pin number on SoC
	 */
	.dqs_map[LP4_CH0] = { 3, 1, 2, 0, 7, 5, 6, 4 },
	.dqs_map[LP4_CH1] = { 3, 2, 0, 1, 7, 5, 6, 4 },

	/* Baseboard uses three 100 Ohm rcomp resistors */
	.rcomp_resistor = { 100, 100, 100 },

	/*
	 * Baseboard Rcomp target values.
	 * Rcomp targets for baseboard should be
	 * { 80, 40, 40, 40, 30 }, but we need to
	 * nil out rcomp targets for now to avoid bug b:70896346
	 */
	.rcomp_targets = { 0, 0, 0, 0, 0 },

	/* Baseboard is a non-interleaved design */
	.dq_pins_interleaved = 0,

	/* Disable Early Command Training */
	.ect = 0,
};

const struct lpddr4_cfg *__weak variant_lpddr4_config(void)
{
	return &baseboard_lpddr4_cfg;
}

size_t __weak variant_memory_sku(void)
{
	const gpio_t pads[] = {
		[3] = GPIO_MEM_CONFIG_3, [2] = GPIO_MEM_CONFIG_2,
		[1] = GPIO_MEM_CONFIG_1, [0] = GPIO_MEM_CONFIG_0,
	};

	return gpio_base2_value(pads, ARRAY_SIZE(pads));
}
