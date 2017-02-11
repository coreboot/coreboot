/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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
#include <device/spi.h>
#include <spi-generic.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

static int spi_acpi_get_bus(struct device *dev)
{
	struct device *spi_dev;
	struct device_operations *ops;

	if (!dev->bus || !dev->bus->dev)
		return -1;

	spi_dev = dev->bus->dev;
	ops = spi_dev->ops;

	if (ops && ops->ops_spi_bus &&
	    ops->ops_spi_bus->dev_to_bus)
		return ops->ops_spi_bus->dev_to_bus(spi_dev);

	return -1;
}

static void spi_acpi_fill_ssdt_generator(struct device *dev)
{
	struct drivers_spi_acpi_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct spi_cfg spi_cfg;
	struct spi_slave slave;
	int bus = -1, cs = dev->path.spi.cs;
	struct acpi_spi spi = {
		.device_select = cs,
		.speed = config->speed ? : 1 * MHz,
		.resource = scope,
	};

	if (!dev->enabled || !scope)
		return;

	bus = spi_acpi_get_bus(dev);
	if (bus == -1) {
		printk(BIOS_ERR, "%s: ERROR: Cannot get bus for device.\n",
		       dev_path(dev));
		return;
	}

	if (!config->hid) {
		printk(BIOS_ERR, "%s: ERROR: HID required.\n", dev_path(dev));
		return;
	}

	if (spi_setup_slave(bus, cs, &slave)) {
		printk(BIOS_ERR, "%s: ERROR: SPI setup failed.\n",
			dev_path(dev));
		return;
	}

	if (spi_get_config(&slave, &spi_cfg)) {
		printk(BIOS_ERR, "%s: ERROR: SPI get config failed.\n",
			dev_path(dev));
		return;
	}

	spi.device_select_polarity = spi_cfg.cs_polarity;
	spi.wire_mode = spi_cfg.wire_mode;
	spi.data_bit_length = spi_cfg.data_bit_length;
	spi.clock_phase = spi_cfg.clk_phase;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", config->hid);
	if (config->cid)
		acpigen_write_name_string("_CID", config->cid);
	acpigen_write_name_integer("_UID", config->uid);
	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_spi(&spi);
	acpi_device_write_interrupt(&config->irq);
	acpigen_write_resourcetemplate_footer();

	if (config->compat_string) {
		struct acpi_dp *dsd = acpi_dp_new_table("_DSD");
		acpi_dp_add_string(dsd, "compatible", config->compat_string);
		acpi_dp_write(dsd);
	}

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *spi_acpi_name(struct device *dev)
{
	struct drivers_spi_acpi_config *config = dev->chip_info;
	static char name[5];

	if (config->name)
		return config->name;

	snprintf(name, sizeof(name), "S%03.3X", spi_acpi_get_bus(dev));
	name[4] = '\0';
	return name;
}

static struct device_operations spi_acpi_ops = {
	.read_resources		  = DEVICE_NOOP,
	.set_resources		  = DEVICE_NOOP,
	.enable_resources	  = DEVICE_NOOP,
	.acpi_name		  = &spi_acpi_name,
	.acpi_fill_ssdt_generator = &spi_acpi_fill_ssdt_generator,
};

static void spi_acpi_enable(struct device *dev)
{
	dev->ops = &spi_acpi_ops;
}

struct chip_operations drivers_spi_acpi_ops = {
	CHIP_NAME("SPI Device")
	.enable_dev = &spi_acpi_enable
};
