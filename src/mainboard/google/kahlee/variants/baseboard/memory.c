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

#include <gpio.h> /* src/include/gpio.h */
#include <baseboard/variants.h>
#include <variant/gpio.h>

uint8_t __attribute__((weak)) variant_memory_sku(void)
{
	gpio_t pads[] = {
		[3] = MEM_CONFIG3,
		[2] = MEM_CONFIG2,
		[1] = MEM_CONFIG1,
		[0] = MEM_CONFIG0,
	};

	return gpio_base2_value(pads, ARRAY_SIZE(pads));
}
