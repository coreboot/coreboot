/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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
#include <soc/gpio.h>

static void early_config_gpio(void)
{
	const struct pad_config *base_early_table;
	const struct pad_config *override_early_table;
	size_t base_gpios;
	size_t override_gpios;

	base_early_table = base_early_gpio_table(&base_gpios);
	override_early_table = override_early_gpio_table(&override_gpios);

	gpio_configure_pads_with_override(base_early_table,
					base_gpios,
					override_early_table,
					override_gpios);
}

void bootblock_mainboard_init(void)
{
	early_config_gpio();
}
