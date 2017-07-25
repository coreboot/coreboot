/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#include <boardid.h>
#include <console/console.h>
#include <gpio.h>

uint8_t board_id(void)
{
	MAYBE_STATIC int id = -1;

	gpio_t gpios[] = {
		GPIO_135,
		GPIO_140,
		GPIO_144,
	};

	if (id < 0) {
		id = gpio_base2_value(gpios, ARRAY_SIZE(gpios));
		printk(BIOS_SPEW, "Board ID: %#x.\n", id);
	}

	return id;
}
