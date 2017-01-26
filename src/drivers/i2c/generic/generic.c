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
#include <device/i2c.h>
#include <device/device.h>
#include <device/path.h>
#include <gpio.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)

static void i2c_generic_add_power_res(struct drivers_i2c_generic_config *config)
{
	const char *power_res_dev_states[] = { "_PR0", "_PR3" };
	unsigned reset_gpio = config->reset_gpio.pins[0];
	unsigned enable_gpio = config->enable_gpio.pins[0];

	if (!config->has_power_resource)
		return;

	if (!reset_gpio && !enable_gpio)
		return;

	/* PowerResource (PRIC, 0, 0) */
	acpigen_write_power_res("PRIC", 0, 0, power_res_dev_states,
				ARRAY_SIZE(power_res_dev_states));

	/* Method (_STA, 0, NotSerialized) { Return (0x1) } */
	acpigen_write_STA(0x1);

	/* Method (_ON, 0, Serialized) */
	acpigen_write_method_serialized("_ON", 0);
	if (reset_gpio)
		acpigen_soc_set_tx_gpio(reset_gpio);
	if (enable_gpio) {
		acpigen_soc_set_tx_gpio(enable_gpio);
		acpigen_write_sleep(config->enable_delay_ms);
	}
	if (reset_gpio) {
		acpigen_soc_clear_tx_gpio(reset_gpio);
		acpigen_write_sleep(config->reset_delay_ms);
	}
	acpigen_pop_len();		/* _ON method */

	/* Method (_OFF, 0, Serialized) */
	acpigen_write_method_serialized("_OFF", 0);
	if (reset_gpio)
		acpigen_soc_set_tx_gpio(reset_gpio);
	if (enable_gpio)
		acpigen_soc_clear_tx_gpio(enable_gpio);
	acpigen_pop_len();		/* _OFF method */

	acpigen_pop_len();		/* PowerResource PRIC */
}

static bool i2c_generic_add_gpios_to_crs(struct drivers_i2c_generic_config *cfg)
{
	/*
	 * Return false if:
	 * 1. Request to explicitly disable export of GPIOs in CRS, or
	 * 2. Both reset and enable GPIOs are not provided.
	 */
	if (cfg->disable_gpio_export_in_crs ||
	    ((cfg->reset_gpio.pin_count == 0) &&
	     (cfg->enable_gpio.pin_count == 0)))
		return false;

	return true;
}

static int i2c_generic_write_gpio(struct acpi_gpio *gpio, int *curr_index)
{
	int ret = -1;

	if (gpio->pin_count == 0)
		return ret;

	acpi_device_write_gpio(gpio);
	ret = *curr_index;
	(*curr_index)++;

	return ret;
}

void i2c_generic_fill_ssdt(struct device *dev,
			void (*callback)(struct device *dev),
			struct drivers_i2c_generic_config *config)
{
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dsd = NULL;
	int curr_index = 0;
	int reset_gpio_index = -1, enable_gpio_index = -1;
	const char *path = acpi_device_path(dev);

	if (!dev->enabled || !scope)
		return;

	if (!config->hid) {
		printk(BIOS_ERR, "%s: ERROR: HID required\n", dev_path(dev));
		return;
	}

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", config->hid);
	if (config->cid)
		acpigen_write_name_string("_CID", config->cid);
	acpigen_write_name_integer("_UID", config->uid);
	acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	acpi_device_write_interrupt(&config->irq);
	if (i2c_generic_add_gpios_to_crs(config) == true) {
		reset_gpio_index = i2c_generic_write_gpio(&config->reset_gpio,
							&curr_index);
		enable_gpio_index = i2c_generic_write_gpio(&config->enable_gpio,
							&curr_index);
	}
	acpigen_write_resourcetemplate_footer();

	/* Wake capabilities */
	if (config->wake) {
		acpigen_write_name_integer("_S0W", 4);
		acpigen_write_PRW(config->wake, 3);
	}

	/* DSD */
	if (config->probed || (reset_gpio_index != -1) ||
		(enable_gpio_index != -1)) {
		dsd = acpi_dp_new_table("_DSD");
		if (config->probed)
			acpi_dp_add_integer(dsd, "linux,probed", 1);
		if (reset_gpio_index != -1)
			acpi_dp_add_gpio(dsd, "reset-gpios", path,
					reset_gpio_index, 0,
					config->reset_gpio.polarity);
		if (enable_gpio_index != -1)
			acpi_dp_add_gpio(dsd, "enable-gpios", path,
					enable_gpio_index, 0,
					config->enable_gpio.polarity);
		acpi_dp_write(dsd);
	}

	/* Power Resource */
	i2c_generic_add_power_res(config);

	/* Callback if any. */
	if (callback)
		callback(dev);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", path,
	       config->desc ? : dev->chip_ops->name, dev_path(dev));
}

static void i2c_generic_fill_ssdt_generator(struct device *dev)
{
	i2c_generic_fill_ssdt(dev, NULL, dev->chip_info);
}

/* Use name specified in config or build one from I2C address */
static const char *i2c_generic_acpi_name(struct device *dev)
{
	struct drivers_i2c_generic_config *config = dev->chip_info;
	static char name[5];

	if (config->name)
		return config->name;

	snprintf(name, sizeof(name), "D%03.3X", dev->path.i2c.device);
	name[4] = '\0';
	return name;
}
#endif

static struct device_operations i2c_generic_ops = {
	.read_resources		  = DEVICE_NOOP,
	.set_resources		  = DEVICE_NOOP,
	.enable_resources	  = DEVICE_NOOP,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name		  = &i2c_generic_acpi_name,
	.acpi_fill_ssdt_generator = &i2c_generic_fill_ssdt_generator,
#endif
};

static void i2c_generic_enable(struct device *dev)
{
	struct drivers_i2c_generic_config *config = dev->chip_info;

	/* Check if device is present by reading GPIO */
	if (config->device_present_gpio) {
		int present = gpio_get(config->device_present_gpio);
		present ^= config->device_present_gpio_invert;

		printk(BIOS_INFO, "%s is %spresent\n",
		       dev->chip_ops->name, present ? "" : "not ");

		if (!present) {
			dev->enabled = 0;
			return;
		}
	}

	dev->ops = &i2c_generic_ops;
}

struct chip_operations drivers_i2c_generic_ops = {
	CHIP_NAME("I2C Device")
	.enable_dev = &i2c_generic_enable
};
