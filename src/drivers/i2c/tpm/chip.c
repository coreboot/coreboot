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
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>
#include <stdint.h>
#include <string.h>
#include "tpm.h"
#include "chip.h"

static void i2c_tpm_fill_ssdt(struct device *dev)
{
	struct drivers_i2c_tpm_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};

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
	acpigen_write_name_integer("_UID", config->uid);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	if (config->irq_gpio.pin_count)
		acpi_device_write_gpio(&config->irq_gpio);
	else
		acpi_device_write_interrupt(&config->irq);

	acpigen_write_resourcetemplate_footer();

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev),
	       dev->chip_ops->name, dev_path(dev));
}

static const char *i2c_tpm_acpi_name(const struct device *dev)
{
	return "TPMI";
}

static struct device_operations i2c_tpm_ops = {
	.read_resources		  = DEVICE_NOOP,
	.set_resources		  = DEVICE_NOOP,
	.enable_resources	  = DEVICE_NOOP,
	.acpi_name		  = &i2c_tpm_acpi_name,
	.acpi_fill_ssdt_generator = &i2c_tpm_fill_ssdt,
};

static void i2c_tpm_enable(struct device *dev)
{
	struct drivers_i2c_tpm_config *config = dev->chip_info;

	dev->ops = &i2c_tpm_ops;

	if (config && config->desc) {
		dev->name = config->desc;
	}
}

struct chip_operations drivers_i2c_tpm_ops = {
	CHIP_NAME("I2C TPM")
	.enable_dev = &i2c_tpm_enable
};
