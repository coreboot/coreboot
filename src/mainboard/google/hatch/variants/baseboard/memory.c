/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
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
#include <gpio.h>
#include <soc/cnl_memcfg_init.h>

static const struct cnl_mb_cfg baseboard_memcfg = {
	/*
	 * The dqs_map arrays map the ddr4 pins to the SoC pins
	 * for both channels.
	 *
	 * the index = pin number on ddr4 part
	 * the value = pin number on SoC
	 */
	.dqs_map[DDR_CH0] = { 0, 1, 4, 5, 2, 3, 6, 7 },
	.dqs_map[DDR_CH1] = { 0, 1, 4, 5, 2, 3, 6, 7 },

	/* Baseboard uses 120, 81 and 100 rcomp resistors */
	.rcomp_resistor = { 120, 81, 100 },

	/* Baseboard Rcomp target values */
	.rcomp_targets = { 100, 40, 20, 20, 26 },

	/* Set CaVref config to 2 */
	.vref_ca_config = 2,

	/* Enable Early Command Training */
	.ect = 1,
};

const struct cnl_mb_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int __weak variant_memory_sku(void)
{
	const gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}
