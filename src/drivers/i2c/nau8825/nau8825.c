/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>
#include <stdint.h>
#include "chip.h"

#if CONFIG(HAVE_ACPI_TABLES)

#define NAU8825_ACPI_NAME	"NAU8"
#define NAU8825_ACPI_HID	"10508825"

#define NAU8825_DP_INT(key,val) \
	acpi_dp_add_integer(dp, "nuvoton," key, (val))

static void nau8825_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_nau8825_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dp = NULL;

	if (!scope)
		return;
	if (config->sar_threshold_num > NAU8825_MAX_BUTTONS)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", NAU8825_ACPI_HID);
	acpigen_write_name_integer("_UID", 0);
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
	NAU8825_DP_INT("jkdet-enable", config->jkdet_enable);
	NAU8825_DP_INT("jkdet-pull-enable", config->jkdet_pull_enable);
	NAU8825_DP_INT("jkdet-pull-up", config->jkdet_pull_up);
	NAU8825_DP_INT("jkdet-polarity", config->jkdet_polarity);
	NAU8825_DP_INT("vref-impedance", config->vref_impedance);
	NAU8825_DP_INT("micbias-voltage", config->micbias_voltage);
	NAU8825_DP_INT("sar-hysteresis", config->sar_hysteresis);
	NAU8825_DP_INT("sar-voltage", config->sar_voltage);
	NAU8825_DP_INT("sar-compare-time", config->sar_compare_time);
	NAU8825_DP_INT("sar-sampling-time", config->sar_sampling_time);
	NAU8825_DP_INT("short-key-debounce", config->short_key_debounce);
	NAU8825_DP_INT("jack-insert-debounce", config->jack_insert_debounce);
	NAU8825_DP_INT("jack-eject-deboune", config->jack_eject_debounce);
	NAU8825_DP_INT("sar-threshold-num", config->sar_threshold_num);
	NAU8825_DP_INT("adcout-drive-strong", config->adcout_ds ? 1 : 0);
	acpi_dp_add_integer_array(dp, "nuvoton,sar-threshold",
			  config->sar_threshold, config->sar_threshold_num);
	acpi_dp_write(dp);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s address 0%xh irq %d\n",
	       acpi_device_path(dev), dev->chip_ops->name,
	       dev->path.i2c.device, config->irq.pin);
}

static const char *nau8825_acpi_name(const struct device *dev)
{
	return NAU8825_ACPI_NAME;
}
#endif

static struct device_operations nau8825_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name              = nau8825_acpi_name,
	.acpi_fill_ssdt		= nau8825_fill_ssdt,
#endif
};

static void nau8825_enable(struct device *dev)
{
	dev->ops = &nau8825_ops;
}

struct chip_operations drivers_i2c_nau8825_ops = {
	CHIP_NAME("Nuvoton NAU8825 Codec")
	.enable_dev = nau8825_enable
};
