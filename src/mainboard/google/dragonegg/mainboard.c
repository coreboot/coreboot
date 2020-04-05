/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/ec.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <variant/gpio.h>

static void mainboard_init(void *chip_info)
{
	size_t num;
	const struct pad_config *gpio_table;

	gpio_table = variant_gpio_table(&num);
	gpio_configure_pads(gpio_table, num);

	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->write_acpi_tables = NULL;
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
