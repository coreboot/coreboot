/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include "board.h"
#include <commonlib/bsd/cb_err.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/socinfo.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	gpio_t pins[3] = { 0 };
	if (CONFIG(BOARD_GOOGLE_HEROBRINE_REV0)) {
		pins[2] = GPIO(75);
		pins[1] = GPIO(74);
		pins[0] = GPIO(73);
	} else {
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
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	/*Update modem status in 9th bit of sku id*/
	uint32_t mask = 1 << 9;
	id = google_chromeec_get_board_sku();
	id = ((id & ~mask) | (socinfo_modem_supported() << 9));
	return id;
}
