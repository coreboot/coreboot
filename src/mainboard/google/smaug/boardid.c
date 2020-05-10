/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <soc/sdram.h>

#include "gpio.h"

uint32_t board_id(void)
{
	static int id = -1;

	if (id < 0) {
		gpio_t gpio[] = {[1] = BD_ID1, [0] = BD_ID0};	/* ID0 is LSB */

		id = gpio_base3_value(gpio, ARRAY_SIZE(gpio));
	}

	return id;
}

uint32_t ram_code(void)
{
	return sdram_get_ram_code();
}
