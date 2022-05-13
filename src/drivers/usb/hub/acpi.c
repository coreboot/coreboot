/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <stdlib.h>

#include "chip.h"

static const char *usb_hub_acpi_name(const struct device *dev)
{
	const char *pattern;
	struct drivers_usb_hub_config *config = dev->chip_info;

	if (config->acpi_name[0] != 0)
		return config->acpi_name;

	/* USB ACPI driver does not have acpi_name operation defined. Hence return
	   the ACPI name for both the hub and any downstream facing ports. */
	switch (dev->path.usb.port_type) {
	case 0:
		return "EHUB";
	case 2:
		pattern = "HS%02d";
		break;
	case 3:
		pattern = "SS%02d";
		break;
	default:
		return NULL;
	}

	snprintf(config->acpi_name, sizeof(config->acpi_name), pattern,
		 dev->path.usb.port_id + 1);

	return config->acpi_name;
}

static void usb_hub_add_ports(const struct device *dev)
{
	const struct drivers_usb_hub_config *config = config_of(dev);
	struct device *port = NULL;
	unsigned int child_count = 0;

	while ((port = dev_bus_each_child(dev->link_list, port)) != NULL) {
		if (child_count++ >= config->port_count) {
			printk(BIOS_WARNING, "%s cannot be added. Port Count limit reached.\n",
			       dev_name(port));
			continue;
		}
		acpigen_write_device(usb_hub_acpi_name(port));
		acpigen_write_name_byte("_ADR", port->path.usb.port_id + 1);
		acpigen_write_device_end();
	}
}

static void usb_hub_acpi_fill_ssdt(const struct device *dev)
{
	const struct drivers_usb_hub_config *config = config_of(dev);
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);

	acpigen_write_scope(scope);
	acpigen_write_device(name);
	acpigen_write_ADR(0);
	if (config->name)
		acpigen_write_name_string("_DDN", config->name);
	if (config->desc)
		acpigen_write_name_unicode("_STR", config->desc);
	usb_hub_add_ports(dev);
	acpigen_write_device_end();
	acpigen_write_scope_end();
}

static struct device_operations usb_hub_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.scan_bus	= scan_static_bus,
	.acpi_fill_ssdt	= usb_hub_acpi_fill_ssdt,
	.acpi_name	= usb_hub_acpi_name
};

static void usb_hub_acpi_enable(struct device *dev)
{
	dev->ops = &usb_hub_ops;
}

struct chip_operations drivers_usb_hub_ops = {
	 CHIP_NAME("USB Hub")
	.enable_dev = usb_hub_acpi_enable
};
