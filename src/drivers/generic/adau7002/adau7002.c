/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include "chip.h"

#if CONFIG(HAVE_ACPI_TABLES)

#define ADAU7002_ACPI_NAME	"ADAU"
#define ADAU7002_ACPI_HID	"ADAU7002"

static void adau7002_fill_ssdt(const struct device *dev)
{
	struct drivers_generic_adau7002_config *config;
	struct acpi_dp *dp;

	if (!dev)
		return;

	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);
	if (!scope || !name)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(name);
	acpigen_write_name_string("_HID", ADAU7002_ACPI_HID);
	acpigen_write_name_integer("_UID", 0);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	/* _DSD for devicetree properties */
	config = dev->chip_info;
	dp = acpi_dp_new_table("_DSD");
	acpi_dp_add_integer(dp, "wakeup-delay-ms", config->wakeup_delay);
	acpi_dp_write(dp);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s\n", acpi_device_path(dev),
	       dev->chip_ops->name);
}

static const char *adau7002_acpi_name(const struct device *dev)
{
	return ADAU7002_ACPI_NAME;
}
#endif

static struct device_operations adau7002_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= adau7002_acpi_name,
	.acpi_fill_ssdt		= adau7002_fill_ssdt,
#endif
};

static void adau7002_enable(struct device *dev)
{
	dev->ops = &adau7002_ops;
}

struct chip_operations drivers_generic_adau7002_ops = {
	CHIP_NAME("Analog Digital DMIC")
	.enable_dev = adau7002_enable
};
