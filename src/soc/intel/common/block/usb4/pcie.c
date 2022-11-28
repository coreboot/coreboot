/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <string.h>

#include "chip.h"

#if CONFIG(HAVE_ACPI_TABLES)
static void usb4_pcie_acpi_fill_ssdt(const struct device *dev)
{
	const struct soc_intel_common_block_usb4_config *config;
	const struct device *parent;
	struct acpi_dp *dsd;
	const char *usb4_path;
	int port_id;

	/* Get parent PCI device */
	parent = dev->bus->dev;
	if (!parent) {
		printk(BIOS_ERR, "%s: Unable to find parent device\n", __func__);
		return;
	}

	if (!parent->enabled)
		return;

	config = config_of(dev);
	if (!config->usb4_port) {
		printk(BIOS_ERR, "%s: Unable to find reference to usb4_port\n", __func__);
		return;
	}

	/* Get ACPI path to USB4 device. */
	usb4_path = acpi_device_path(config->usb4_port);
	if (!usb4_path) {
		printk(BIOS_ERR, "%s: Unable to find ACPI path for usb4_port %s\n",
		       __func__, dev_path(config->usb4_port));
		return;
	}

	usb4_path = strdup(usb4_path);
	port_id = dev->path.generic.id;

	acpigen_write_scope(acpi_device_path(parent));

	/* Add pointer to USB4 port controller. */
	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_reference(dsd, "usb4-host-interface", usb4_path);
	acpi_dp_add_integer(dsd, "usb4-port-number", port_id);

	/* Indicate that device supports hotplug in D3. */
	acpi_device_add_hotplug_support_in_d3(dsd);

	/* Indicate that port is external. */
	acpi_device_add_external_facing_port(dsd);

	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(parent),
	       config->desc ? : dev->chip_ops->name, dev_path(parent));
}
#endif

static struct device_operations usb4_pcie_acpi_dev_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt		= usb4_pcie_acpi_fill_ssdt,
#endif
};

static void usb4_pcie_acpi_enable(struct device *dev)
{
	dev->ops = &usb4_pcie_acpi_dev_ops;
}

struct chip_operations soc_intel_common_block_usb4_ops = {
	CHIP_NAME("Intel USB4 PCIe Root Port")
	.enable_dev = usb4_pcie_acpi_enable
};
