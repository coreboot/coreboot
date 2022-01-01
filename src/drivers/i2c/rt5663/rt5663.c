/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c.h>
#include <device/device.h>
#include <device/path.h>
#include "chip.h"

#define RT5663_ACPI_NAME	"RT53"
#define RT5663_ACPI_HID		"10EC5663"

#define RT5663_DP_INT(key, val) \
	acpi_dp_add_integer(dp, "realtek," key, (val))

static void rt5663_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_rt5663_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dp;

	if (!scope)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", RT5663_ACPI_HID);
	acpigen_write_name_integer("_UID", config->uid);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	/* Allow either GpioInt() or Interrupt() */
	if (config->irq_gpio.pin_count)
		acpi_device_write_gpio(&config->irq_gpio);
	else
		acpi_device_write_interrupt(&config->irq);
	acpigen_write_resourcetemplate_footer();

	/* Device Properties */
	dp = acpi_dp_new_table("_DSD");
	if (config->irq_gpio.pin_count)
		acpi_dp_add_gpio(dp, "irq-gpios", acpi_device_path(dev), 0, 0,
			 config->irq_gpio.active_low);
	RT5663_DP_INT("dc_offset_l_manual", config->dc_offset_l_manual);
	RT5663_DP_INT("dc_offset_r_manual", config->dc_offset_r_manual);
	RT5663_DP_INT("dc_offset_l_manual_mic", config->dc_offset_l_manual_mic);
	RT5663_DP_INT("dc_offset_r_manual_mic", config->dc_offset_r_manual_mic);
	acpi_dp_write(dp);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s address 0%xh\n", acpi_device_path(dev),
			dev->chip_ops->name, dev->path.i2c.device);
}

static const char *rt5663_acpi_name(const struct device *dev)
{
	return RT5663_ACPI_NAME;
}

static struct device_operations rt5663_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= rt5663_acpi_name,
	.acpi_fill_ssdt		= rt5663_fill_ssdt,
};

static void rt5663_enable(struct device *dev)
{
	dev->ops = &rt5663_ops;
}

struct chip_operations drivers_i2c_rt5663_ops = {
	CHIP_NAME("Realtek RT5663 Codec")
	.enable_dev = rt5663_enable
};
