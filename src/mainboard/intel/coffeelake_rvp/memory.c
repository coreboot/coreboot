/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
 * Copyright 2018 Intel Corp.
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
#include <soc/cnl_memcfg_init.h>

static const struct cnl_mb_cfg baseboard_memcfg_cfg = {
	/*
	 * The dqs_map arrays map the ddr4 pins to the SoC pins
	 * for both channels.
	 *
	 * the index = pin number on ddr4 part
	 * the value = pin number on SoC
	 */
	.dqs_map[DDR_CH0] = { 0, 1, 3, 2, 4, 5, 6, 7 },
	.dqs_map[DDR_CH1] = { 1, 0, 4, 5, 2, 3, 6, 7 },

	/* Baseboard uses 121, 81 and 100 rcomp resistors */
	.rcomp_resistor = { 121, 81, 100 },

	/*
	 * Baseboard Rcomp target values.
	 */
	.rcomp_targets = { 100, 40, 20, 20, 26 },

	/* Baseboard is an interleaved design */
	.dq_pins_interleaved = 1,

	/* Baseboard is using config 2 for vref_ca */
	.vref_ca_config = 2,

	/* Disable Early Command Training */
	.ect = 0,
};

const struct cnl_mb_cfg *__weak variant_memcfg_config(void)
{
	return &baseboard_memcfg_cfg;
}
