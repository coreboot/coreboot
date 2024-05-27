/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <stdio.h>
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

	if (key->wakeup_route == WAKEUP_ROUTE_SCI)
		acpigen_write_PRW(key->wake_gpe, 3);

	if (key->wakeup_route != WAKEUP_ROUTE_DISABLED) {
		acpi_dp_add_integer(dsd, "wakeup-source", 1);
		acpi_dp_add_integer(dsd, "wakeup-event-action",
					key->wakeup_event_action);
	}

	if (key->can_be_disabled)
		acpi_dp_add_integer(dsd, "linux,can-disable",
				    key->can_be_disabled);
	if (key->debounce_interval)
		acpi_dp_add_integer(dsd, "debounce-interval",
				    key->debounce_interval);
	acpi_dp_add_gpio(dsd, "gpios", parent_path, 0, 0,
			 config->gpio.active_low);

	return dsd;
}

static void gpio_keys_fill_ssdt_generator(const struct device *dev)
{
	struct drivers_generic_gpio_keys_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);
	struct acpi_dp *dsd, *child;
	const char *drv_string = config->is_polled ? "gpio-keys-polled"
				: "gpio-keys";

	if (!scope || !path || !config->gpio.pin_count)
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
	if (config->label)
		acpi_dp_add_string(dsd, "label", config->label);
	if (config->is_polled)
		acpi_dp_add_integer(dsd, "poll-interval",
					config->poll_interval);
	/* Child device defining key */
	child = gpio_keys_add_child_node(config, path);
	if (child)
		acpi_dp_add_child(dsd, "button-0", child);
	acpi_dp_write(dsd);

	acpigen_write_STA(acpi_device_status(dev));

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
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= gpio_keys_acpi_name,
	.acpi_fill_ssdt		= gpio_keys_fill_ssdt_generator,
};

static void gpio_keys_enable(struct device *dev)
{
	dev->ops = &gpio_keys_ops;
}

struct chip_operations drivers_generic_gpio_keys_ops = {
	.name = "GPIO Keys",
	.enable_dev = gpio_keys_enable
};
