/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include "board.h"
#include <commonlib/bsd/cb_err.h>
#include <console/console.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	gpio_t pins[3];
	if (CONFIG(BOARD_GOOGLE_HEROBRINE)) {
		pins[2] = GPIO(75);
		pins[1] = GPIO(74);
		pins[0] = GPIO(73);
	} else if (CONFIG(BOARD_GOOGLE_PIGLIN) || CONFIG(BOARD_GOOGLE_HOGLIN)
			|| CONFIG(BOARD_GOOGLE_SENOR)) {
		pins[2] = GPIO(50);
		pins[1] = GPIO(49);
		pins[0] = GPIO(48);
	}

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base3_value(pins, ARRAY_SIZE(pins));

	printk(BIOS_INFO, "BoardID :%d - "
				"Machine model: "
				"Qualcomm Technologies, Inc. "
				"sc7280 platform\n", id);

	return id;
}

uint32_t ram_code(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	return id;
}

uint32_t sku_id(void)
{
	return google_chromeec_get_board_sku();
}
