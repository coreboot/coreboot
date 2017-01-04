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

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <ec/ec.h>
#include <soc/lpc.h>
#include <soc/gpio.h>
#include <variant/ec.h>

void bootblock_mainboard_init(void)
{
	const struct pad_config *pads;
	size_t num;

	lpc_configure_pads();
	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);
	mainboard_ec_init();
}
