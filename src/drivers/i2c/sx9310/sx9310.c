/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <stdio.h>

#include "chip.h"

#define I2C_SX9310_ACPI_ID	"STH9310"
#define I2C_SX9310_ACPI_NAME	"Semtech SX9310"

static const char * const i2c_sx9310_resolution[] = {
	[SX9310_COARSEST] = "coarsest",
	[SX9310_VERY_COARSE] = "very-coarse",
	[SX9310_COARSE] = "coarse",
	[SX9310_MEDIUM_COARSE] = "medium-coarse",
	[SX9310_MEDIUM] = "medium",
	[SX9310_FINE] = "fine",
	[SX9310_VERY_FINE] = "very-fine",
	[SX9310_FINEST] = "finest",
};

static void i2c_sx9310_fill_ssdt(const struct device *dev)
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
	struct acpi_dp *combined_sensors;

	if (!scope || !config)
		return;

	if (config->speed)
		i2c.speed = config->speed;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", I2C_SX9310_ACPI_ID);
	acpigen_write_name_integer("_UID", config->uid);
	acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(ACPI_STATUS_DEVICE_HIDDEN_ON);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);

	if (config->irq_gpio.pin_count)
		acpi_device_write_gpio(&config->irq_gpio);
	else
		acpi_device_write_interrupt(&config->irq);

	acpigen_write_resourcetemplate_footer();

	/* DSD */
	dsd = acpi_dp_new_table("_DSD");

	/*
	 * Format describe in linux kernel documentation. See
	 * https://www.kernel.org/doc/Documentation/devicetree/bindings/iio/proximity/semtech%2Csx9310.yaml
	 */
	acpi_dp_add_integer(dsd, "semtech,cs0-ground", config->cs0_ground);
	acpi_dp_add_integer(dsd, "semtech,startup-sensor",
			config->startup_sensor);
	acpi_dp_add_integer(dsd, "semtech,proxraw-strength",
			config->proxraw_strength);
	acpi_dp_add_integer(dsd, "semtech,avg-pos-strength",
			config->avg_pos_strength);

	/* Add combined_sensors package */
	if (config->combined_sensors_count > 0) {
		combined_sensors = acpi_dp_new_table("semtech,combined-sensors");
		for (int i = 0;
				i < config->combined_sensors_count &&
				i < MAX_COMBINED_SENSORS_ENTRIES; ++i) {
			acpi_dp_add_integer(combined_sensors, NULL,
					    config->combined_sensors[i]);
		}
		acpi_dp_add_array(dsd, combined_sensors);
	}
	if (config->resolution && config->resolution < ARRAY_SIZE(i2c_sx9310_resolution))
		acpi_dp_add_string(dsd, "semtech,resolution",
				   i2c_sx9310_resolution[config->resolution]);

	acpi_dp_write(dsd);
	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev),
	       config->desc ? : dev->chip_ops->name, dev_path(dev));
}

static const char *i2c_sx9310_acpi_name(const struct device *dev)
{
	static char name[5];

	snprintf(name, sizeof(name), "D%03.3X", dev->path.i2c.device);
	return name;
}

static struct device_operations i2c_sx9310_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= i2c_sx9310_acpi_name,
	.acpi_fill_ssdt		= i2c_sx9310_fill_ssdt,
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
	.name = I2C_SX9310_ACPI_NAME,
	.enable_dev = i2c_sx9310_enable
};
