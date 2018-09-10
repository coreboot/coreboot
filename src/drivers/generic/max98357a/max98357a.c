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
#include <gpio.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)

#define MAX98357A_ACPI_NAME	"MAXM"
#define MAX98357A_ACPI_HID	"MX98357A"

static void max98357a_fill_ssdt(struct device *dev)
{
	struct drivers_generic_max98357a_config *config = dev->chip_info;
	const char *path;
	struct acpi_dp *dp;

	if (!dev->enabled || !config)
		return;

	/* Device */
	acpigen_write_scope(acpi_device_scope(dev));
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", MAX98357A_ACPI_HID);
	acpigen_write_name_integer("_UID", 0);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_gpio(&config->sdmode_gpio);
	acpigen_write_resourcetemplate_footer();

	/* _DSD for devicetree properties */
	/* This points to the first pin in the first gpio entry in _CRS */
	path = acpi_device_path(dev);
	dp = acpi_dp_new_table("_DSD");
	acpi_dp_add_gpio(dp, "sdmode-gpio", path, 0, 0,
			 config->sdmode_gpio.polarity);
	acpi_dp_add_integer(dp, "sdmode-delay", config->sdmode_delay);
	acpi_dp_write(dp);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s\n", path, dev->chip_ops->name);
}

static const char *max98357a_acpi_name(const struct device *dev)
{
	return MAX98357A_ACPI_NAME;
}
#endif

static struct device_operations max98357a_ops = {
	.read_resources		  = DEVICE_NOOP,
	.set_resources		  = DEVICE_NOOP,
	.enable_resources	  = DEVICE_NOOP,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name                = &max98357a_acpi_name,
	.acpi_fill_ssdt_generator = &max98357a_fill_ssdt,
#endif
};

static void max98357a_enable(struct device *dev)
{
	struct drivers_generic_max98357a_config *config = dev->chip_info;

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

	dev->ops = &max98357a_ops;
}

struct chip_operations drivers_generic_max98357a_ops = {
	CHIP_NAME("Maxim Integrated 98357A Amplifier")
	.enable_dev = &max98357a_enable
};
