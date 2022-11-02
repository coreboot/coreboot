/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>
#include <string.h>
#include "chip.h"

#define I2C_SX9324_ACPI_ID	"STH9324"
#define I2C_SX9324_CHIP_NAME	"Semtech SX9324"

#define REGISTER(NAME) acpi_dp_add_integer(dsd, \
					I2C_SX9324_ACPI_ID "," #NAME, \
					config->NAME)

static void i2c_sx9324_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_sx9324_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dsd;

	if (!scope || !config)
		return;

	if (config->speed)
		i2c.speed = config->speed;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", I2C_SX9324_ACPI_ID);
	acpigen_write_name_integer("_UID", config->uid);
	acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(acpi_device_status(dev));

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
	acpi_dp_add_integer_array(dsd, "semtech,ph0-pin", config->ph0_pin, ARRAY_SIZE(config->ph0_pin));
	acpi_dp_add_integer_array(dsd, "semtech,ph1-pin", config->ph1_pin, ARRAY_SIZE(config->ph1_pin));
	acpi_dp_add_integer_array(dsd, "semtech,ph2-pin", config->ph2_pin, ARRAY_SIZE(config->ph2_pin));
	acpi_dp_add_integer_array(dsd, "semtech,ph3-pin", config->ph3_pin, ARRAY_SIZE(config->ph3_pin));
	acpi_dp_add_integer(dsd, "semtech,ph01-resolution", config->ph01_resolution);
	acpi_dp_add_integer(dsd, "semtech,ph23-resolution", config->ph23_resolution);
	acpi_dp_add_integer(dsd, "semtech,startup-sensor", config->startup_sensor);
	acpi_dp_add_integer(dsd, "semtech,ph01-proxraw-strength", config->ph01_proxraw_strength);
	acpi_dp_add_integer(dsd, "semtech,ph23-proxraw-strength", config->ph23_proxraw_strength);
	acpi_dp_add_integer(dsd, "semtech,avg-pos-strength", config->avg_pos_strength);
	acpi_dp_add_integer(dsd, "semtech,input-precharge-resistor-ohms", config->input_precharge_resistor_ohms);
	acpi_dp_add_integer(dsd, "semtech,input-analog-gain", config->input_analog_gain);
	acpi_dp_add_string(dsd, "semtech,cs-idle-sleep", config->cs_idle_sleep);
	acpi_dp_add_string(dsd, "semtech,int-comp-resistor", config->int_comp_resistor);
#if CONFIG(DRIVERS_I2C_SX9324_SUPPORT_LEGACY_LINUX_DRIVER)
#include "registers.h"
#endif
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev),
	       config->desc ? : dev->chip_ops->name, dev_path(dev));
}

#undef REGISTER

static const char *i2c_sx9324_acpi_name(const struct device *dev)
{
	static char name[5];

	snprintf(name, sizeof(name), "SX%02.2X", dev->path.i2c.device);
	return name;
}

static struct device_operations i2c_sx9324_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= i2c_sx9324_acpi_name,
	.acpi_fill_ssdt		= i2c_sx9324_fill_ssdt,
};

static void i2c_sx9324_enable(struct device *dev)
{
	struct drivers_i2c_sx9324_config *config = dev->chip_info;

	if (!config) {
		dev->enabled = 0;
		return;
	}

	dev->ops = &i2c_sx9324_ops;

	if (config->desc)
		dev->name = config->desc;
}

struct chip_operations drivers_i2c_sx9324_ops = {
	CHIP_NAME(I2C_SX9324_CHIP_NAME)
	.enable_dev = i2c_sx9324_enable
};
