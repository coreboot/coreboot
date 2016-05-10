/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <bootblock_common.h>
#include <soc/lpc.h>
#include <soc/gpio.h>
#include "ec.h"
#include "gpio.h"

void bootblock_mainboard_init(void)
{
	lpc_configure_pads();
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
	mainboard_ec_init();
}
