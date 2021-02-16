/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void mainboard_configure_gpios(void)
{
	size_t base_num_gpios, override_num_gpios;
	const struct soc_amd_gpio *base_gpios, *override_gpios;

	base_gpios = variant_base_gpio_table(&base_num_gpios);
	override_gpios = variant_override_gpio_table(&override_num_gpios);

	gpio_configure_pads_with_override(base_gpios, base_num_gpios, override_gpios,
					  override_num_gpios);
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
