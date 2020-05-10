/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void graphics_fill_ssdt(const struct device *dev)
{
	acpi_device_write_pci_dev(dev);
	pci_rom_ssdt(dev);
}

static const char *graphics_acpi_name(const struct device *dev)
{
	return "IGFX";
}

static const struct device_operations graphics_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
	.write_acpi_tables	= pci_rom_write_acpi_tables,
	.acpi_fill_ssdt		= graphics_fill_ssdt,
	.acpi_name		= graphics_acpi_name,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_FAM17H_GPU,
	0,
};

static const struct pci_driver graphics_driver __pci_driver = {
	.ops		= &graphics_ops,
	.vendor		= PCI_VENDOR_ID_ATI,
	.devices	= pci_device_ids,
};
