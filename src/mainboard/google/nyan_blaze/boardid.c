/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <console/console.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static int id = -1;
	gpio_t gpio[] = {[3] = GPIO(X4), [2] = GPIO(X1),	/* X4 is MSB */
			 [1] = GPIO(T1), [0] = GPIO(Q3),};	/* Q3 is LSB */

	if (id < 0) {
		id = gpio_base3_value(gpio, ARRAY_SIZE(gpio));

		printk(BIOS_SPEW, "Board TRISTATE ID: %d.\n", id);
	}

	return id;
}
