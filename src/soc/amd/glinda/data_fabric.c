/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Glinda */

#include <acpi/acpi_device.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static const char *data_fabric_acpi_name(const struct device *dev)
{
	switch (dev->device) {
	case PCI_DID_AMD_FAM17H_MODELA0H_DF0:
		return "DFD0";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF1:
		return "DFD1";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF2:
		return "DFD2";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF3:
		return "DFD3";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF4:
		return "DFD4";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF5:
		return "DFD5";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF6:
		return "DFD6";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF7:
		return "DFD7";
	default:
		printk(BIOS_ERR, "%s: Unhandled device id 0x%x\n", __func__, dev->device);
	}

	return NULL;
}

struct device_operations glinda_data_fabric_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= data_fabric_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};
