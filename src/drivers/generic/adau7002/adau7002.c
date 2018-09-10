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
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)

#define ADAU7002_ACPI_NAME	"ADAU"
#define ADAU7002_ACPI_HID	"ADAU7002"

static void adau7002_fill_ssdt(struct device *dev)
{
	if (!dev || !dev->enabled)
		return;

	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);
	if (!scope || !name)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(name);
	acpigen_write_name_string("_HID", ADAU7002_ACPI_HID);
	acpigen_write_name_integer("_UID", 0);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s\n", acpi_device_path(dev),
	       dev->chip_ops->name);
}

static const char *adau7002_acpi_name(const struct device *dev)
{
	return ADAU7002_ACPI_NAME;
}
#endif

static struct device_operations adau7002_ops = {
	.read_resources			= DEVICE_NOOP,
	.set_resources			= DEVICE_NOOP,
	.enable_resources		= DEVICE_NOOP,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name			= &adau7002_acpi_name,
	.acpi_fill_ssdt_generator	= &adau7002_fill_ssdt,
#endif
};

static void adau7002_enable(struct device *dev)
{
	dev->ops = &adau7002_ops;
}

struct chip_operations drivers_generic_adau7002_ops = {
	CHIP_NAME("Analog Digital DMIC")
	.enable_dev = &adau7002_enable
};
