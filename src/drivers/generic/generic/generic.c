/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

static void generic_dev_fill_ssdt_generator(struct device *dev)
{
	struct acpi_dp *dsd;
	struct drivers_generic_generic_config *config = dev->chip_info;

	if (!dev->enabled || !config)
		return;

	if (!config->hid) {
		printk(BIOS_ERR, "%s: ERROR: _HID required\n", dev_path(dev));
		return;
	}

	acpigen_write_scope(acpi_device_scope(dev));
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", config->hid);
	if (config->cid)
		acpigen_write_name_string("_CID", config->cid);
	acpigen_write_name_integer("_UID", config->uid);
	acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(acpi_device_status(dev));

	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_property_list(dsd, config->property_list,
				  config->property_count);
	acpi_dp_write(dsd);

	acpigen_pop_len();	/* Device */
	acpigen_pop_len();	/* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev),
	       config->desc ? : dev->chip_ops->name, dev_path(dev));
}

static char *generic_autogen_name(struct drivers_generic_generic_config *config)
{
	char *name = &config->autogen_name[0];
	static unsigned int id;

	if (name[0] != '\0')
		return name;

	snprintf(name, sizeof(name), "G%03.3X", id++);
	name[4] = '\0';
	return name;
}

static const char *generic_dev_acpi_name(const struct device *dev)
{
	struct drivers_generic_generic_config *config = dev->chip_info;

	if (config->name)
		return config->name;

	return generic_autogen_name(config);
}

static struct device_operations generic_dev_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= generic_dev_acpi_name,
	.acpi_fill_ssdt		= generic_dev_fill_ssdt_generator,
};

static void generic_dev_enable(struct device *dev)
{
	dev->ops = &generic_dev_ops;
}

struct chip_operations drivers_generic_generic_ops = {
	CHIP_NAME("Generic Device")
	.enable_dev = generic_dev_enable
};
