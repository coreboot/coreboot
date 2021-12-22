/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>

#include "chip.h"

#define CS35L53_ACPI_HID	"CSC3541"

static void cs35l53_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_cs35l53_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dsd;
	int gpio_index = 0;

	if (!scope)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", CS35L53_ACPI_HID);
	acpigen_write_name_integer("_UID", 0);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_name_string("_SUB", config->sub);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	/* Use either Interrupt() or GpioInt() */
	if (config->irq_gpio.pin_count)
		acpi_device_write_gpio(&config->irq_gpio);
	else
		acpi_device_write_interrupt(&config->irq);

	/* for cs35l53 reset gpio */
	if (config->reset_gpio.pin_count)
		acpi_device_write_gpio(&config->reset_gpio);

	acpigen_write_resourcetemplate_footer();

	/* Add Child Device Properties */
	dsd = acpi_dp_new_table("_DSD");
	if (config->irq_gpio.pin_count)
		acpi_dp_add_gpio(dsd, "irq-gpios", path,
			 gpio_index++,  /* Index = 0 */
			 0,  /* Pin = 0 (There is a single pin in the GPIO resource). */
			 config->irq_gpio.active_low);
	if (config->reset_gpio.pin_count)
		acpi_dp_add_gpio(dsd, "reset-gpios", path,
			 gpio_index++,  /* Index = 0 or 1 (if irq gpio is written). */
			 0,  /* Pin = 0 (There is a single pin in the GPIO resource). */
			 config->reset_gpio.active_low);

	acpi_dp_add_integer(dsd, "cirrus,boost-type", config->boost_type);

	switch (config->boost_type) {
	case INTERNAL_BOOST:
		if ((config->boost_peak_milliamp > 4500) ||
		(config->boost_peak_milliamp < 1600) ||
		(config->boost_peak_milliamp % 50)) {
			printk(BIOS_ERR,
			       "%s: Incorrect boost_peak_milliamp(%d). Using default of 4500 mA\n",
				__func__, config->boost_peak_milliamp);
			config->boost_peak_milliamp = 4500;
		}
		acpi_dp_add_integer(dsd, "cirrus,boost-peak-milliamp",
				    config->boost_peak_milliamp);
		acpi_dp_add_integer(dsd, "cirrus,boost-ind-nanohenry",
				    config->boost_ind_nanohenry);
		acpi_dp_add_integer(dsd, "cirrus,boost-cap-microfarad",
				    config->boost_cap_microfarad);
		break;
	case EXTERNAL_BOOST:
		config->gpio1_output_enable = true;
		config->gpio1_src_select = GPIO1_SRC_GPIO;
		break;
	default:
		break;
	}

	acpi_dp_add_integer(dsd, "cirrus,asp-sdout-hiz", config->asp_sdout_hiz);
	acpi_dp_add_integer(dsd, "cirrus,gpio1-polarity-invert",
				config->gpio1_polarity_invert);
	acpi_dp_add_integer(dsd, "cirrus,gpio1-output-enable",
				config->gpio1_output_enable);
	acpi_dp_add_integer(dsd, "cirrus,gpio1-src-select", config->gpio1_src_select);
	acpi_dp_add_integer(dsd, "cirrus,gpio2-polarity-invert",
				config->gpio2_polarity_invert);
	acpi_dp_add_integer(dsd, "cirrus,gpio2-output-enable",
				config->gpio2_output_enable);
	acpi_dp_add_integer(dsd, "cirrus,gpio2-src-select", config->gpio2_src_select);

	/* Write Device Property Hierarchy */
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s address 0%xh irq %d\n",
	       acpi_device_path(dev), dev->chip_ops->name,
	       dev->path.i2c.device, config->irq.pin);
}

static const char *cs35l53_acpi_name(const struct device *dev)
{
	struct drivers_i2c_cs35l53_config *config = dev->chip_info;
	static char name[ACPI_NAME_BUFFER_SIZE];

	if (config->name)
		return config->name;

	snprintf(name, sizeof(name), "D%03.3X", dev->path.i2c.device);
	return name;
}

static struct device_operations cs35l53_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= cs35l53_acpi_name,
	.acpi_fill_ssdt		= cs35l53_fill_ssdt,
};

static void cs35l53_enable(struct device *dev)
{
	dev->ops = &cs35l53_ops;
}

struct chip_operations drivers_i2c_cs35l53_ops = {
	CHIP_NAME("Cirrus Logic CS35L53 Audio Codec")
	.enable_dev = cs35l53_enable
};
