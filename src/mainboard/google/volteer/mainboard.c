/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/ec.h>
#include <fw_config.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <variant/gpio.h>

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static void add_fw_config_oem_string(const struct fw_config *config, void *arg)
{
	struct smbios_type11 *t;
	char buffer[64];

	t = (struct smbios_type11 *)arg;

	snprintf(buffer, sizeof(buffer), "%s-%s", config->field_name, config->option_name);
	t->count = smbios_add_string(t->eos, buffer);
}

static void mainboard_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	fw_config_for_each_found(add_fw_config_oem_string, t);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
	dev->ops->get_smbios_strings = mainboard_smbios_strings;
}

static void mainboard_chip_init(void *chip_info)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	base_pads = variant_base_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num,
		override_pads, override_num);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
