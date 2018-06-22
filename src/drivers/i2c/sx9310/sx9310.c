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

#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

#define I2C_SX9310_ACPI_ID	"SX9310"
#define I2C_SX9310_ACPI_NAME	"Semtech SX9310"

#define REGISTER(NAME) acpi_dp_add_integer(dsd, \
					I2C_SX9310_ACPI_ID "," #NAME, \
					config->NAME)

static void i2c_sx9310_fill_ssdt(struct device *dev)
{
	struct drivers_i2c_sx9310_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dsd;

	if (!dev->enabled || !scope || !config)
		return;

	if (config->speed)
		i2c.speed = config->speed;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", I2C_SX9310_ACPI_ID);
	acpigen_write_name_integer("_UID", config->uid);
	acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	acpi_device_write_interrupt(&config->irq);
	acpigen_write_resourcetemplate_footer();

	/* DSD */
	dsd = acpi_dp_new_table("_DSD");
#include "registers.h"
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

#undef REGISTER

static const char *i2c_sx9310_acpi_name(const struct device *dev)
{
	static char name[5];

	snprintf(name, sizeof(name), "D%03.3X", dev->path.i2c.device);
	return name;
}

static struct device_operations i2c_sx9310_ops = {
	.read_resources		  = DEVICE_NOOP,
	.set_resources		  = DEVICE_NOOP,
	.enable_resources	  = DEVICE_NOOP,
	.acpi_name		  = &i2c_sx9310_acpi_name,
	.acpi_fill_ssdt_generator = &i2c_sx9310_fill_ssdt,
};

static void i2c_sx9310_enable(struct device *dev)
{
	struct drivers_i2c_sx9310_config *config = dev->chip_info;

	if (!config) {
		dev->enabled = 0;
		return;
	}

	dev->ops = &i2c_sx9310_ops;

	if (config->desc)
		dev->name = config->desc;
}

struct chip_operations drivers_i2c_sx9310_ops = {
	CHIP_NAME(I2C_SX9310_ACPI_NAME)
	.enable_dev = &i2c_sx9310_enable
};
