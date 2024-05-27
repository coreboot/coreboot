/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen_dsm.h>
#include <acpi/acpi_device.h>
#include <assert.h>
#include <device/device.h>
#include <stdio.h>

#include "chip.h"
#include <gpio.h>
#include <console/console.h>

#if CONFIG(HAVE_ACPI_TABLES)
static void i2c_hid_fill_dsm(const struct device *dev)
{
	struct drivers_i2c_hid_config *config = dev->chip_info;
	struct dsm_i2c_hid_config dsm_config = {
		.hid_desc_reg_offset = config->hid_desc_reg_offset,
	};

	acpigen_write_dsm_i2c_hid(&dsm_config);
}

static void i2c_hid_fill_ssdt_generator(const struct device *dev)
{
	struct drivers_i2c_hid_config *config = dev->chip_info;
	config->generic.cid = I2C_HID_CID;
	i2c_generic_fill_ssdt(dev, &i2c_hid_fill_dsm, &config->generic);
}

static const char *i2c_hid_acpi_name(const struct device *dev)
{
	static char name[5];
	struct drivers_i2c_hid_config *config = dev->chip_info;
	if (config->generic.name)
		return config->generic.name;

	snprintf(name, sizeof(name), "H%03.3X", dev->path.i2c.device);
	name[4] = '\0';
	return name;
}
#endif

static struct device_operations i2c_hid_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= i2c_hid_acpi_name,
	.acpi_fill_ssdt		= i2c_hid_fill_ssdt_generator,
#endif
};

static void i2c_hid_enable(struct device *dev)
{
	struct drivers_i2c_hid_config *config = dev->chip_info;

	if (!config)
		return;

	/* Check if device is present by reading GPIO */
	if (config->generic.device_present_gpio) {
		int present = gpio_get(config->generic.device_present_gpio);
		present ^= config->generic.device_present_gpio_invert;

		printk(BIOS_INFO, "%s is %spresent\n",
		       dev->chip_ops->name, present ? "" : "not ");

		if (!present) {
			dev->enabled = 0;
			return;
		}
	}

	/*
	 * Ensure that I2C HID devices use level triggered interrupts as per ACPI
	 * I2C HID requirement. Check interrupt and GPIO interrupt.
	 */
	if ((!config->generic.irq_gpio.pin_count &&
	      config->generic.irq.mode != ACPI_IRQ_LEVEL_TRIGGERED) ||
	    (config->generic.irq_gpio.pin_count &&
	     config->generic.irq_gpio.irq.mode != ACPI_IRQ_LEVEL_TRIGGERED)) {
		printk(BIOS_ERR, "%s IRQ is not level triggered.\n", config->generic.hid);
		BUG();
	}

	dev->ops = &i2c_hid_ops;

	if (config && config->generic.desc) {
		dev->name = config->generic.desc;
	}
}

struct chip_operations drivers_i2c_hid_ops = {
	.name = "I2C HID Device",
	.enable_dev = i2c_hid_enable
};
