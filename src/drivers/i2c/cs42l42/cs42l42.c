/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>

#include "chip.h"

#define CS42L42_ACPI_NAME	"CRUS"
#define CS42L42_ACPI_HID	"10134242"

static void cs42l42_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_cs42l42_config *config = dev->chip_info;
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
	acpigen_write_name_string("_HID", CS42L42_ACPI_HID);
	acpigen_write_name_integer("_UID", 0);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
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

	/* for cs42l42reset gpio */
	if (config->reset_gpio.pin_count)
		acpi_device_write_gpio(&config->reset_gpio);

	acpigen_write_resourcetemplate_footer();

	/* AAD Child Device Properties */
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
	acpi_dp_add_integer(dsd, "cirrus,ts-inv", config->ts_inv ? 1 : 0);
	acpi_dp_add_integer(dsd, "cirrus,ts-dbnc-rise", config->ts_dbnc_rise);
	acpi_dp_add_integer(dsd, "cirrus,ts-dbnc-fall", config->ts_dbnc_fall);
	acpi_dp_add_integer(dsd, "cirrus,btn-det-init-dbnce", config->btn_det_init_dbnce);

	if (config->btn_det_init_dbnce > 200) {
		printk(BIOS_ERR, "%s: Incorrect btn_det_init_dbnce(%d). Using default of 100ms\n",
				__func__, config->btn_det_init_dbnce);
		config->btn_det_init_dbnce = 100;
	}

	acpi_dp_add_integer(dsd, "cirrus,btn-det-event-dbnce", config->btn_det_event_dbnce);

	if (config->btn_det_event_dbnce > 100) {
		printk(BIOS_ERR, "%s: Incorrect btn_det_event_dbnce(%d). Using default of 10ms\n",
				__func__, config->btn_det_event_dbnce);
		config->btn_det_event_dbnce = 10;
	}

	acpi_dp_add_integer_array(dsd, "cirrus,bias-lvls", config->bias_lvls, 4);
	acpi_dp_add_integer(dsd, "cirrus,hs-bias-ramp-rate", config->hs_bias_ramp_rate);
	if (config->hs_bias_sense_disable)
		acpi_dp_add_integer(dsd, "cirrus,hs-bias-sense-disable", 1);

	/* Write Device Property Hierarchy */
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s address 0%xh irq %d\n",
	       acpi_device_path(dev), dev->chip_ops->name,
	       dev->path.i2c.device, config->irq.pin);
}

static const char *cs42l42_acpi_name(const struct device *dev)
{
	return CS42L42_ACPI_NAME;
}

static struct device_operations cs42l42_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= cs42l42_acpi_name,
	.acpi_fill_ssdt		= cs42l42_fill_ssdt,
};

static void cs42l42_enable(struct device *dev)
{
	dev->ops = &cs42l42_ops;
}

struct chip_operations drivers_i2c_cs42l42_ops = {
	CHIP_NAME("Cirrus Logic CS42l42 Audio Codec")
	.enable_dev = cs42l42_enable
};
