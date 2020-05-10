/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <gpio.h>
#include <console/console.h>

/*
 * Gale boards dedicate to the board ID three GPIOs in ternary mode: 64, 65
 * and 66.
 */

static int board_id_value = -1;

static uint8_t get_board_id(void)
{
	uint8_t bid;
	gpio_t hw_rev_gpios[] = {[2] = 66, [1] = 65, [0] = 64};	/* 64 is LSB */

	bid = gpio_binary_first_base3_value(hw_rev_gpios,
		ARRAY_SIZE(hw_rev_gpios));
	printk(BIOS_INFO, "Board ID %d\n", bid);

	return bid;
}

uint32_t board_id(void)
{
	if (board_id_value < 0)
		board_id_value = get_board_id();

	return board_id_value;
}
