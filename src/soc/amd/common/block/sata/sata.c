/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <amdblocks/sata.h>

static const char *sata_acpi_name(const struct device *dev)
{
	return "STCR";
}

struct device_operations amd_sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= soc_enable_sata_features,
	.ops_pci		= &pci_dev_ops_pci,
	.acpi_name		= sata_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};
