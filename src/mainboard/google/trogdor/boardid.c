/*
 * This file is part of the coreboot project.
 *
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

#include <boardid.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	const gpio_t pins[] = {[2] = GPIO(31), [1] = GPIO(93), [0] = GPIO(33)};

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));

	return id;
}

uint32_t ram_code(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	const gpio_t pins[] = {[1] = GPIO(91), [0] = GPIO(29)};

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));

	return id;
}

uint32_t sku_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	const gpio_t pins[] = {[1] = GPIO(90), [0] = GPIO(114)};

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));

	return id;
}
