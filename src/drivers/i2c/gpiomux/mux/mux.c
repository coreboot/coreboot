/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <stdlib.h>
#include <string.h>
#include "chip.h"

static const char *i2c_gpiomux_mux_acpi_name(const struct device *dev)
{
	static char name[ACPI_NAME_BUFFER_SIZE];

	snprintf(name, ACPI_NAME_BUFFER_SIZE, "MUX%01.1X", dev->path.generic.id);
	return name;
}

static void i2c_gpiomux_mux_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);
	struct drivers_i2c_gpiomux_mux_config *config = config_of(dev);
	struct acpi_dp *dsd = NULL;
	const char *compat_string = "i2c-mux-gpio";
	struct acpi_gpio_res_params param[MAX_NUM_MUX_GPIOS];
	int i;

	if (!scope || !path)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", ACPI_DT_NAMESPACE_HID);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	for (i = 0; i < config->mux_gpio_count; i++) {
		acpi_device_write_gpio(&config->mux_gpio[i]);
		param[i].ref = path;
		param[i].index = i;
		param[i].pin = 0;
		param[i].active_low = config->mux_gpio[i].active_low;
	}
	acpigen_write_resourcetemplate_footer();

	/* DSD */
	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_string(dsd, "compatible", compat_string);
	acpi_dp_add_gpio_array(dsd, "mux-gpios", param, config->mux_gpio_count);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", path, dev->chip_ops->name, dev_path(dev));
}

static struct device_operations i2c_gpiomux_mux_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.scan_bus		= scan_static_bus,
	.acpi_name		= i2c_gpiomux_mux_acpi_name,
	.acpi_fill_ssdt		= i2c_gpiomux_mux_fill_ssdt,
};

static void i2c_gpiomux_mux_enable(struct device *dev)
{
	if (!dev)
		return;

	dev->ops = &i2c_gpiomux_mux_ops;
}

struct chip_operations drivers_i2c_gpiomux_mux_ops = {
	CHIP_NAME("I2C GPIO MUX Device")
	.enable_dev = i2c_gpiomux_mux_enable
};
