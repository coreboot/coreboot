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

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <soc/gpio.h>

static void early_config_gpio(void)
{
	const struct pad_config *variant_early_table;
	size_t variant_gpios;

	variant_early_table = variant_early_gpio_table(&variant_gpios);
	gpio_configure_pads(variant_early_table, variant_gpios);
}

void bootblock_mainboard_init(void)
{
	early_config_gpio();
}
