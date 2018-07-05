/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google Inc.
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

static struct acpi_dp *gpio_keys_add_child_node(
			struct drivers_generic_gpio_keys_config *config,
			const char *parent_path)
{
	struct key_info *key = &config->key;
	struct acpi_dp *dsd;

	if (!key->dev_name || !key->linux_code)
		return NULL;

	dsd = acpi_dp_new_table(config->key.dev_name);

	acpi_dp_add_integer(dsd, "linux,code", key->linux_code);
	if (key->linux_input_type)
		acpi_dp_add_integer(dsd, "linux,input-type",
				    key->linux_input_type);
	if (key->label)
		acpi_dp_add_string(dsd, "label", key->label);
	if (key->is_wakeup_source)
		acpi_dp_add_integer(dsd, "wakeup-source",
				    key->is_wakeup_source);
	if (key->can_be_disabled)
		acpi_dp_add_integer(dsd, "linux,can-disable",
				    key->can_be_disabled);
	if (key->debounce_interval)
		acpi_dp_add_integer(dsd, "debounce-interval",
				    key->debounce_interval);
	acpi_dp_add_gpio(dsd, "gpios", parent_path, 0, 0,
			 config->gpio.polarity);

	return dsd;
}

static void gpio_keys_fill_ssdt_generator(struct device *dev)
{
	struct drivers_generic_gpio_keys_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);
	struct acpi_dp *dsd, *child;
	const char *drv_string = config->is_polled ? "gpio-keys-polled"
				: "gpio-keys";

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
	acpi_dp_add_string(dsd, "compatible", drv_string);
	if (config->is_polled)
		acpi_dp_add_integer(dsd, "poll-interval",
					config->poll_interval);
	/* Child device defining key */
	child = gpio_keys_add_child_node(config, path);
	if (child)
		acpi_dp_add_child(dsd, "button-0", child);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *gpio_keys_acpi_name(const struct device *dev)
{
	struct drivers_generic_gpio_keys_config *config = dev->chip_info;
	static char name[5];

	if (config->name)
		return config->name;

	snprintf(name, sizeof(name), "K%03.3X", config->gpio.pins[0]);
	name[4] = '\0';

	return name;
}

static struct device_operations gpio_keys_ops = {
	.read_resources			= DEVICE_NOOP,
	.set_resources			= DEVICE_NOOP,
	.enable_resources		= DEVICE_NOOP,
	.acpi_name			= &gpio_keys_acpi_name,
	.acpi_fill_ssdt_generator	= &gpio_keys_fill_ssdt_generator,
};

static void gpio_keys_enable(struct device *dev)
{
	dev->ops = &gpio_keys_ops;
}

struct chip_operations drivers_generic_gpio_keys_ops = {
	CHIP_NAME("GPIO Keys")
	.enable_dev = &gpio_keys_enable
};
