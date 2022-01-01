/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>
#include "chip.h"

#define MAX98373_ACPI_NAME	"MAXI"
#define MAX98373_ACPI_HID	"MX98373"

static void max98373_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_max98373_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dp;

	if (!scope) {
		printk(BIOS_ERR, "%s: dev not enabled\n", __func__);
		return;
	}

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", MAX98373_ACPI_HID);
	acpigen_write_name_integer("_UID", config->uid);
	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	acpigen_write_resourcetemplate_footer();

	/* Device Properties */
	dp = acpi_dp_new_table("_DSD");

	if (config->interleave_mode)
		acpi_dp_add_integer(dp, "maxim,interleave_mode",
				config->interleave_mode);
	acpi_dp_add_integer(dp, "maxim,vmon-slot-no", config->vmon_slot_no);
	acpi_dp_add_integer(dp, "maxim,imon-slot-no", config->imon_slot_no);

	acpi_dp_write(dp);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s address 0%xh\n", acpi_device_path(dev),
			dev->chip_ops->name, dev->path.i2c.device);
}

static const char *max98373_acpi_name(const struct device *dev)
{
	struct drivers_i2c_max98373_config *config = dev->chip_info;

	if (config->name)
		return config->name;

	return MAX98373_ACPI_NAME;
}

static struct device_operations max98373_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= max98373_acpi_name,
	.acpi_fill_ssdt		= max98373_fill_ssdt,
};

static void max98373_enable(struct device *dev)
{
	struct drivers_i2c_max98373_config *config = dev->chip_info;

	dev->ops = &max98373_ops;

	if (config && config->desc) {
		dev->name = config->desc;
	}
}

struct chip_operations drivers_i2c_max98373_ops = {
	CHIP_NAME("Maxim MAX98373 Codec")
	.enable_dev = max98373_enable
};
