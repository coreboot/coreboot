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
#include <arch/acpi_pld.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

static void usb_acpi_fill_ssdt_generator(struct device *dev)
{
	struct drivers_usb_acpi_config *config = dev->chip_info;
	const char *path = acpi_device_path(dev);

	if (!dev->enabled || !path || !config)
		return;

	/* Don't generate output for hubs, only ports */
	if (config->type == UPC_TYPE_HUB)
		return;

	acpigen_write_scope(path);
	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_upc(config->type);

	if (config->use_custom_pld) {
		/* Use board defined PLD */
		acpigen_write_pld(&config->custom_pld);
	} else {
		/* Fill PLD strucutre based on port type */
		struct acpi_pld pld;
		acpi_pld_fill_usb(&pld, config->type);
		acpigen_write_pld(&pld);
	}

	acpigen_pop_len();

	printk(BIOS_INFO, "%s: %s at %s\n", path,
	       config->desc ? : dev->chip_ops->name, dev_path(dev));
}

static struct device_operations usb_acpi_ops = {
	.read_resources			= DEVICE_NOOP,
	.set_resources			= DEVICE_NOOP,
	.enable_resources		= DEVICE_NOOP,
	.scan_bus			= &scan_usb_bus,
	.acpi_fill_ssdt_generator	= &usb_acpi_fill_ssdt_generator,
};

static void usb_acpi_enable(struct device *dev)
{
	dev->ops = &usb_acpi_ops;
}

struct chip_operations drivers_usb_acpi_ops = {
	CHIP_NAME("USB ACPI Device")
	.enable_dev = &usb_acpi_enable
};
