/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <stdlib.h>
#include <string.h>
#include "chip.h"

static const char *i2c_gpiomux_bus_acpi_name(const struct device *dev)
{
	static char name[ACPI_NAME_BUFFER_SIZE];

	snprintf(name, ACPI_NAME_BUFFER_SIZE, "MXA%01.1X", dev->path.generic.id);
	return name;
}

static void i2c_gpiomux_bus_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);

	if (!dev || !dev->enabled || !scope || !path)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	acpigen_write_STA(acpi_device_status(dev));
	acpigen_write_ADR(dev->path.generic.id);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", path, dev->chip_ops->name, dev_path(dev));
}

static struct device_operations i2c_gpiomux_bus_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.scan_bus		= scan_static_bus,
	.acpi_name		= i2c_gpiomux_bus_acpi_name,
	.acpi_fill_ssdt		= i2c_gpiomux_bus_fill_ssdt,
};

static void i2c_gpiomux_bus_enable(struct device *dev)
{
	if (!dev)
		return;

	dev->ops = &i2c_gpiomux_bus_ops;
}

struct chip_operations drivers_i2c_gpiomux_bus_ops = {
	CHIP_NAME("I2C GPIO MUX Bus Device")
	.enable_dev = i2c_gpiomux_bus_enable
};
