/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <boardid.h>
#include <gpio.h>

uint32_t board_id(void)
{
	const gpio_t pins[] = {[2] = GPIO(51), [1] = GPIO(62), [0] = GPIO(38)};
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));

	return id;
}

uint32_t ram_code(void)
{
	const gpio_t pins[] = {[1] = GPIO(147), [0] = GPIO(146)};
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));

	return id;
}

uint32_t sku_id(void)
{
	const gpio_t pins[] = {[1] = GPIO(113), [0] = GPIO(79)};
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));

	return id;
}
