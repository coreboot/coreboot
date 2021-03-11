/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include "chip.h"

static void alc1015_fill_ssdt(const struct device *dev)
{
	struct drivers_generic_alc1015_config *config = dev->chip_info;
	const char *path;
	struct acpi_dp *dp;

	if (!config)
		return;

	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);
	if (!scope || !name)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(name);

	acpigen_write_name_string("_HID", "RTL1015");
	acpigen_write_name_integer("_UID", 0);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_gpio(&config->sdb);
	acpigen_write_resourcetemplate_footer();

	/* _DSD for devicetree properties */
	/* This points to the first pin in the first gpio entry in _CRS */
	path = acpi_device_path(dev);
	dp = acpi_dp_new_table("_DSD");
	acpi_dp_add_gpio(dp, "sdb-gpios", path, 0, 0, config->sdb.active_low);
	acpi_dp_write(dp);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s\n", path, dev->chip_ops->name);
}

static const char *alc1015_acpi_name(const struct device *dev)
{
	return "ALCP";
}

static struct device_operations alc1015_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= alc1015_acpi_name,
	.acpi_fill_ssdt		= alc1015_fill_ssdt,
};

static void alc1015_enable(struct device *dev)
{
	dev->ops = &alc1015_ops;
}

struct chip_operations drivers_generic_alc1015_ops = {
	CHIP_NAME("ASoC RT1015P Amplifier driver")
	.enable_dev = alc1015_enable
};
