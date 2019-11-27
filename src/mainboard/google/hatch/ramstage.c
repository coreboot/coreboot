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

#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <ec/ec.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	variant_devtree_update();
}

void __weak variant_devtree_update(void)
{
	/* Override dev tree settings per board */
}

void __weak variant_ramstage_init(void)
{
	/* Default weak implementation */
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
}

static void mainboard_chip_init(void *chip_info)
{
	const struct pad_config *base_table;
	const struct pad_config *override_table;
	size_t base_gpios;
	size_t override_gpios;

	base_table = base_gpio_table(&base_gpios);
	override_table = override_gpio_table(&override_gpios);

	gpio_configure_pads_with_override(base_table,
					base_gpios,
					override_table,
					override_gpios);

	variant_ramstage_init();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
