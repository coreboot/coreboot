/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>
#include "chip.h"

#define PCI_HOTPLUG_IN_D3_UUID	"6211E2C0-58A3-4AF3-90E1-927A4E0C55A4"
#define PCI_EXTERNAL_PORT_UUID	"EFCC06CC-73AC-4BC3-BFF0-76143807C389"

#if CONFIG(HAVE_ACPI_TABLES)
static void usb4_pcie_fill_ssdt(const struct device *dev)
{
	const struct soc_intel_common_block_usb4_config *config;
	const struct device *parent;
	struct acpi_dp *dsd, *pkg;
	const char *usb4_path;
	int port_id;

	/* Get parent PCI device */
	parent = dev->bus->dev;
	if (!parent) {
		printk(BIOS_ERR, "%s: Unable to find parent device\n", __func__);
		return;
	}

	if (!dev->enabled || !parent->enabled)
		return;

	config = config_of(dev);
	if (!config->usb4_port) {
		printk(BIOS_ERR, "%s: Unable to find reference to usb4_port\n", __func__);
		return;
	}

	/* Get ACPI path to USB4 device. */
	usb4_path = acpi_device_path(config->usb4_port);
	if (!usb4_path) {
		printk(BIOS_ERR, "%s: Unable to find ACPI path for usb4_port\n", __func__);
		return;
	}

	usb4_path = strdup(usb4_path);
	port_id = dev->path.generic.id;

	acpigen_write_scope(acpi_device_path(dev));

	/* Add pointer to USB4 port controller. */
	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_reference(dsd, "usb4-host-interface", usb4_path);
	acpi_dp_add_integer(dsd, "usb4-port-number", port_id);

	/* Indicate that device supports hotplug in D3. */
	pkg = acpi_dp_new_table(PCI_HOTPLUG_IN_D3_UUID);
	acpi_dp_add_integer(pkg, "HotPlugSupportInD3", 1);
	acpi_dp_add_package(dsd, pkg);

	/* Indicate that port is external. */
	pkg = acpi_dp_new_table(PCI_EXTERNAL_PORT_UUID);
	acpi_dp_add_integer(pkg, "ExternalFacingPort", 1);
	acpi_dp_add_integer(pkg, "UID", port_id);

	acpi_dp_add_package(dsd, pkg);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev), config->desc, dev_path(dev));
}
#endif

static struct device_operations usb4_dev_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_static_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt		= usb4_pcie_fill_ssdt,
#endif
};

static void pcie_enable(struct device *dev)
{
	dev->ops = &usb4_dev_ops;
}

struct chip_operations soc_intel_common_block_usb4_ops = {
	CHIP_NAME("Intel USB4 Root Port")
	.enable_dev = pcie_enable
};
