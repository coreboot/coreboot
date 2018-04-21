/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
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

#include <compiler.h>
#include <baseboard/variants.h>
#include <soc/ramstage.h>

#include <variant/gpio.h>

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	const struct pad_config *pads;
	size_t num;
	variant_devtree_update();
	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);
	pads = variant_sku_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

void __weak variant_devtree_update(void)
{
	/* Override dev tree settings per board */
}
