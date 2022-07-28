/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>

#include "chip.h"

#define MAX98396_ACPI_HID "ADS8396"

static void max98396_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_max98396_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dp;
	const char *path = acpi_device_path(dev);

	if (!scope) {
		printk(BIOS_ERR, "%s: dev not enabled\n", __func__);
		return;
	}

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", MAX98396_ACPI_HID);
	acpigen_write_name_integer("_UID", config->uid);
	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	if (config->reset_gpio.pin_count)
		acpi_device_write_gpio(&config->reset_gpio);
	acpigen_write_resourcetemplate_footer();

	/* Device Properties */
	dp = acpi_dp_new_table("_DSD");

	acpi_dp_add_integer(dp, "adi,vmon-slot-no", config->vmon_slot_no);
	acpi_dp_add_integer(dp, "adi,imon-slot-no", config->imon_slot_no);
	acpi_dp_add_integer(dp, "adi,spkfb-slot-no", config->spkfb_slot_no);
	if (config->reset_gpio.pin_count)
		acpi_dp_add_gpio(dp, "reset-gpios", path,
				0,  /* Index = 0 */
				0,  /* Pin = 0 */
				config->reset_gpio.active_low);
	acpi_dp_write(dp);

	acpigen_write_device_end();
	acpigen_write_scope_end();

	printk(BIOS_INFO, "%s: %s address 0%xh\n", acpi_device_path(dev),
		dev->chip_ops->name, dev->path.i2c.device);
}

static const char *max98396_acpi_name(const struct device *dev)
{
	struct drivers_i2c_max98396_config *config = dev->chip_info;
	static char name[ACPI_NAME_BUFFER_SIZE];

	if (config->name && strlen(config->name) == 4)
		return config->name;

	snprintf(name, sizeof(name), "D%03.3X", dev->path.i2c.device);
	return name;
}

static struct device_operations max98396_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= max98396_acpi_name,
	.acpi_fill_ssdt		= max98396_fill_ssdt,
};

static void max98396_enable(struct device *dev)
{
	struct drivers_i2c_max98396_config *config = dev->chip_info;

	dev->ops = &max98396_ops;

	if (config->desc)
		dev->name = config->desc;
}

struct chip_operations drivers_i2c_max98396_ops = {
	CHIP_NAME("Maxim MAX98396 Codec")
	.enable_dev = max98396_enable
};
