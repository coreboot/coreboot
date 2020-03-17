/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include "gpio.h"

static void mainboard_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;

	/* Configure GPIOs in Ramstage */
	pads = gpio_table(&num);
	gpio_configure_pads(pads, num);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
