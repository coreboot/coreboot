/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <stdint.h>
#include "chip.h"

#if CONFIG(HAVE_ACPI_TABLES)

#define TAS2563_ACPI_NAME	"TXNW"
#define TAS2563_ACPI_HID	"TXNW2563"

static unsigned int count_valid_audio_slots(struct drivers_i2c_tas2563_config *config)
{
	unsigned int count = 0;

	for (unsigned int i = 0; i < TAS2563_MAX_AUDIO_SLOTS; i++) {
		if (config->audio_slots[i] != 0)
			count++;
		else
			break;
	}

	return count;
}

static void tas2563_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_tas2563_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dp = NULL;
	unsigned int slots_count = 0;

	if (!scope)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", TAS2563_ACPI_HID);
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

	if (config->reset_gpio.pin_count)
		acpi_device_write_gpio(&config->reset_gpio);

	acpigen_write_resourcetemplate_footer();

	slots_count = count_valid_audio_slots(config);

	/* Device Properties */
	if (slots_count > 0) {
		dp = acpi_dp_new_table("_DSD");
		acpi_dp_add_integer_array(dp, "ti,audio-slots",
				  config->audio_slots, slots_count);
		acpi_dp_write(dp);
	}
	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s address 0%xh irq %d\n",
	       acpi_device_path(dev), dev->chip_ops->name,
	       dev->path.i2c.device, config->irq.pin);
}

static const char *tas2563_acpi_name(const struct device *dev)
{
	return TAS2563_ACPI_NAME;
}
#endif

static struct device_operations tas2563_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name              = tas2563_acpi_name,
	.acpi_fill_ssdt		= tas2563_fill_ssdt,
#endif
};

static void tas2563_enable(struct device *dev)
{
	dev->ops = &tas2563_ops;
}

struct chip_operations drivers_i2c_tas2563_ops = {
	.name = "TI TAS2563 Audio Amplifier",
	.enable_dev = tas2563_enable
};
