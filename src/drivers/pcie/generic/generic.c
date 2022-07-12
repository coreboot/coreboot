/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <device/device.h>
#include <device/pci.h>
#include "chip.h"

static const char *pcie_generic_acpi_name(const struct device *dev)
{
	struct drivers_pcie_generic_config *config = dev->chip_info;

	if (!config || !config->name)
		return "DEV0";

	return config->name;
}

static void pcie_generic_fill_ssdt(const struct device *dev)
{
	struct drivers_pcie_generic_config *config;

	if (!is_dev_enabled(dev))
		return;

	pci_rom_ssdt(dev);

	config = dev->chip_info;
	if (!config || !dev->bus || !dev->bus->dev)
		return;

	const char *scope;
	const char *name;

	scope = acpi_device_path(dev->bus->dev);
	name = acpi_device_name(dev);

	acpigen_write_scope(scope);
	acpigen_write_device(name);
	acpigen_write_ADR_pci_device(dev);

	acpigen_write_device_end();
	acpigen_write_scope_end();

	printk(BIOS_INFO, "%s.%s: Enable ACPI properties for %s (%s)\n", scope, name,
		dev_path(dev), dev->chip_ops->name);
}

struct device_operations pcie_generic_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
	.acpi_name		= pcie_generic_acpi_name,
	.acpi_fill_ssdt		= pcie_generic_fill_ssdt,
};

static void pcie_generic_enable(struct device *dev)
{
	dev->ops = &pcie_generic_ops;
}

struct chip_operations drivers_pcie_generic_ops = {
	CHIP_NAME("PCIe Device")
	.enable_dev = pcie_generic_enable
};
