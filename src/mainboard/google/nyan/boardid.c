/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <boardid.h>
#include <console/console.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static int id = -1;

	if (id < 0) {
		id = gpio_get(GPIO(Q3)) << 0 |
		     gpio_get(GPIO(T1)) << 1 |
		     gpio_get(GPIO(X1)) << 2 |
		     gpio_get(GPIO(X4)) << 3;
		printk(BIOS_SPEW, "Board ID: %#x.\n", id);
	}

	return id;
}
