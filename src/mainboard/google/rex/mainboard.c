/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void mainboard_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;
	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	/* TODO: Add mainboard-specific SSDT entries */
}

static void mainboard_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	/* TODO: Add mainboard-smbios entries */
}

static void mainboard_dev_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_dev_init;
	dev->ops->get_smbios_strings = mainboard_smbios_strings;
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
