/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <gpio.h>
#include <types.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	const gpio_t pins[] = {[2] = GPIO(31), [1] = GPIO(93), [0] = GPIO(33)};

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));

	return id;
}

/* Whether a revision was built before or after the great pin migration of August 2020. */
static bool use_old_pins(void)
{
	return ((CONFIG(BOARD_GOOGLE_TROGDOR) && board_id() < 2) ||
		(CONFIG(BOARD_GOOGLE_LAZOR) && board_id() < 3) ||
		(CONFIG(BOARD_GOOGLE_POMPOM) && board_id() < 1));
}

uint32_t ram_code(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	const gpio_t old_pins[] = {[2] = GPIO(13), [1] = GPIO(19), [0] = GPIO(29)};
	const gpio_t pins[] = {[2] = GPIO(5), [1] = GPIO(3), [0] = GPIO(1)};

	if (id == UNDEFINED_STRAPPING_ID) {
		if (use_old_pins())
			id = gpio_base2_value(old_pins, ARRAY_SIZE(old_pins));
		else
			id = gpio_base2_value(pins, ARRAY_SIZE(pins));
	}

	return id;
}

uint32_t sku_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	const gpio_t old_pins[] = {[2] = GPIO(20), [1] = GPIO(90), [0] = GPIO(105)};
	const gpio_t pins[] = {[2] = GPIO(2), [1] = GPIO(90), [0] = GPIO(58)};

	if (id == UNDEFINED_STRAPPING_ID) {
		if (use_old_pins())
			id = gpio_base2_value(old_pins, ARRAY_SIZE(old_pins));
		else
			id = gpio_base2_value(pins, ARRAY_SIZE(pins));
	}

	return id;
}
