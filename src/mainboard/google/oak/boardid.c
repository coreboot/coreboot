/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <gpio.h>
#include <console/console.h>
#include "gpio.h"

static int board_id_value = -1;

static uint8_t get_board_id(void)
{
	uint8_t bid = 0;
	static gpio_t pins[] = {[2] = BOARD_ID_2, [1] = BOARD_ID_1,
		[0] = BOARD_ID_0};

	bid = gpio_base2_value(pins, ARRAY_SIZE(pins));

	printk(BIOS_INFO, "Board ID %d\n", bid);

	return bid;
}

uint32_t board_id(void)
{
	if (board_id_value < 0)
		board_id_value = get_board_id();

	return board_id_value;
}

uint32_t ram_code(void)
{
	uint32_t code;
	static gpio_t pins[] = {[3] = RAM_ID_3, [2] = RAM_ID_2, [1] = RAM_ID_1,
		[0] = RAM_ID_0};

	code = gpio_base2_value(pins, ARRAY_SIZE(pins));

	printk(BIOS_INFO, "RAM Config: %u\n", code);

	return code;
}
