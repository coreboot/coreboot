/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google Inc.
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

#include <variant/variant.h>
#include <gpio.h>
#include <variant/gpio.h>

/* Use spd_index array to save mem_id */
static const int spd_index[32] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	4, 3, 6, 1, 0, 0, 0, 0,
	5, 0, 7, 2, 0, 0, 0, 0
	};

int variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
		GPIO_MEM_CONFIG_4,
	};

	return spd_index[gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios))];
}
