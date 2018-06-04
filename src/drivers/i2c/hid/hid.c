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

#include <arch/acpigen_dsm.h>
#include <device/device.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static void i2c_hid_fill_dsm(struct device *dev)
{
	struct drivers_i2c_hid_config *config = dev->chip_info;
	struct dsm_i2c_hid_config dsm_config = {
		.hid_desc_reg_offset = config->hid_desc_reg_offset,
	};

	acpigen_write_dsm_i2c_hid(&dsm_config);
}

static void i2c_hid_fill_ssdt_generator(struct device *dev)
{
	struct drivers_i2c_hid_config *config = dev->chip_info;
	config->generic.cid = I2C_HID_CID;
	i2c_generic_fill_ssdt(dev, &i2c_hid_fill_dsm, &config->generic);
}

static const char *i2c_hid_acpi_name(const struct device *dev)
{
	static char name[5];
	snprintf(name, sizeof(name), "H%03.3X", dev->path.i2c.device);
	name[4] = '\0';
	return name;
}
#endif

static struct device_operations i2c_hid_ops = {
	.read_resources		  = DEVICE_NOOP,
	.set_resources		  = DEVICE_NOOP,
	.enable_resources	  = DEVICE_NOOP,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name		  = &i2c_hid_acpi_name,
	.acpi_fill_ssdt_generator = &i2c_hid_fill_ssdt_generator,
#endif
};

static void i2c_hid_enable(struct device *dev)
{
	struct drivers_i2c_hid_config *config = dev->chip_info;

	dev->ops = &i2c_hid_ops;

	if (config && config->generic.desc) {
		dev->name = config->generic.desc;
	}
}

struct chip_operations drivers_i2c_hid_ops = {
	CHIP_NAME("I2C HID Device")
	.enable_dev = &i2c_hid_enable
};
