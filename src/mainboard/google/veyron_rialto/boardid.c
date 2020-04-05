/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <boardid.h>
#include <console/console.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static int id = -1;
	gpio_t pins[] = {[3] = GPIO(2, A, 7), [2] = GPIO(2, A, 2),
		[1] = GPIO(2, A, 1), [0] = GPIO(2, A, 0)}; /* GPIO2_A0 is LSB */

	if (id < 0) {
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));
		printk(BIOS_SPEW, "Board ID: %d.\n", id);
	}

	return id;
}

uint32_t ram_code(void)
{
	uint32_t code;
	gpio_t pins[] = {[3] = GPIO(8, A, 3), [2] = GPIO(8, A, 2),
		[1] = GPIO(8, A, 1), [0] = GPIO(8, A, 0)}; /* GPIO8_A0 is LSB */

	code = gpio_binary_first_base3_value(pins, ARRAY_SIZE(pins));
	printk(BIOS_SPEW, "RAM Config: %u.\n", code);

	return code;
}
