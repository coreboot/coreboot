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

#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <string.h>

#include "chip.h"

static void gpio_regulator_fill_ssdt_generator(struct device *dev)
{
	struct drivers_generic_gpio_regulator_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);
	struct acpi_dp *dsd;

	if (!dev->enabled || !scope || !path || !config->gpio.pin_count)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	/* _HID is set to PRP0001 */
	acpigen_write_name_string("_HID", ACPI_DT_NAMESPACE_HID);

	/* Resources - _CRS */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_gpio(&config->gpio);
	acpigen_write_resourcetemplate_footer();

	/* DSD */
	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_string(dsd, "compatible", "regulator-fixed");
	acpi_dp_add_string(dsd, "regulator-name", config->name);
	acpi_dp_add_gpio(dsd, "gpio-gpios", path, 0, 0, config->gpio.polarity);
	if (config->enabled_on_boot)
		acpi_dp_add_string(dsd, "regulator-boot-on", "on");
	if (config->gpio.polarity == ACPI_GPIO_ACTIVE_HIGH)
		acpi_dp_add_string(dsd, "enable-active-high", "on");
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *gpio_regulator_acpi_name(struct device *dev)
{
	struct drivers_generic_gpio_regulator_config *config = dev->chip_info;
	static char name[5];

	snprintf(name, sizeof(name), "R%03.3X", config->gpio.pins[0]);
	name[4] = '\0';

	return name;
}

static struct device_operations gpio_regulator_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.acpi_name = &gpio_regulator_acpi_name,
	.acpi_fill_ssdt_generator = &gpio_regulator_fill_ssdt_generator,
};

static void gpio_regulator_enable(struct device *dev)
{
	dev->ops = &gpio_regulator_ops;
}

struct chip_operations drivers_generic_gpio_regulator_ops = {
	CHIP_NAME("GPIO Regulator")
	.enable_dev = &gpio_regulator_enable
};
