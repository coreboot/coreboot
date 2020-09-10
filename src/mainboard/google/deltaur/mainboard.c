/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <variant/gpio.h>

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
}

static void mainboard_chip_init(void *chip_info)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	base_pads = variant_base_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num, override_pads, override_num);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
