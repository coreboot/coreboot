/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <stdio.h>

#include "chip.h"

#define RT5645_ACPI_NAME	"RT58"
#define RT5645_ACPI_HID		"10EC5650"

#define RT5645_DP_INT(key, val) \
	acpi_dp_add_integer(dp, "realtek," key, (val))

static void rt5645_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_rt5645_config *config = dev->chip_info;
	const char *path;
	const char *scope = acpi_device_scope(dev);
	int cbj_sleeve_index = -1, irq_gpio_index = -1, hp_detect_index = -1;
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dp;
	int curr_index = 0;

	if (!config)
		return;

	const char *name = acpi_device_name(dev);
	if (!scope || !name)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(name);

	if (config->hid)
		acpigen_write_name_string("_HID", config->hid);
	else
		acpigen_write_name_string("_HID", RT5645_ACPI_HID);
	acpigen_write_name_integer("_UID", 0);
	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);
	/* Hide the device because of Microsoft Windows */
	acpigen_write_STA(ACPI_STATUS_DEVICE_HIDDEN_ON);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);

	/* Use either Interrupt() or GpioInt() */
	if (config->irq_gpio.pin_count)
		irq_gpio_index = acpi_device_write_dsd_gpio(&config->irq_gpio,
							&curr_index);
	else
		acpi_device_write_interrupt(&config->irq);

	/* Add I2C GPIO index  */
	cbj_sleeve_index = acpi_device_write_dsd_gpio(&config->cbj_sleeve,
							&curr_index);
	hp_detect_index = acpi_device_write_dsd_gpio(&config->hp_detect,
							&curr_index);
	acpigen_write_resourcetemplate_footer();

	/* _DSD for devicetree properties */
	/* This points to the first pin in the first gpio entry in _CRS */
	path = acpi_device_path(dev);
	dp = acpi_dp_new_table("_DSD");
	if (config->irq_gpio.pin_count)
		acpi_dp_add_gpio(dp, "irq-gpios", path, irq_gpio_index, 0,
			config->irq_gpio.active_low);
	if (config->cbj_sleeve.pin_count)
		acpi_dp_add_gpio(dp, "cbj-sleeve-gpios", path, cbj_sleeve_index, 0,
			config->cbj_sleeve.active_low);
	if (config->hp_detect.pin_count)
		acpi_dp_add_gpio(dp, "hp-detect-gpios", path, hp_detect_index, 0,
			config->hp_detect.active_low);
	RT5645_DP_INT("jd-mode", config->jd_mode);
	acpi_dp_write(dp);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s address 0%xh\n", path,
		config->desc ? : dev->chip_ops->name, dev->path.i2c.device);
}

static const char *rt5645_acpi_name(const struct device *dev)
{
	struct drivers_i2c_rt5645_config *config = dev->chip_info;
	static char name[5];
	if (config->name)
		return config->name;

	snprintf(name, sizeof(name), "D%03.3X", dev->path.i2c.device);
	return name;
}

static struct device_operations rt5645_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= rt5645_acpi_name,
	.acpi_fill_ssdt		= rt5645_fill_ssdt,
};

static void rt5645_enable(struct device *dev)
{
	dev->ops = &rt5645_ops;
}

struct chip_operations drivers_i2c_rt5645_ops = {
	.name = "ASoC RT5645 Codec driver",
	.enable_dev = rt5645_enable
};
