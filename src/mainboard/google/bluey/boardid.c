/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	if (id != UNDEFINED_STRAPPING_ID)
		return id;

	const gpio_t pins[] = {
		[3] = GPIO(138),
		[2] = GPIO(137),
		[1] = GPIO(136),
		[0] = GPIO(135)
	};

	id = gpio_base3_value(pins, ARRAY_SIZE(pins));

	return id;
}

uint32_t sku_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	if (id != UNDEFINED_STRAPPING_ID)
		return id;

	if (CONFIG(EC_GOOGLE_CHROMEEC))
		id = google_chromeec_get_board_sku();

	return id;
}
