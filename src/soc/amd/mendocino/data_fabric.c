/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/pci_devs.h>

static const char *data_fabric_acpi_name(const struct device *dev)
{
	const char *df_acpi_names[8] = {
		"DFD0",
		"DFD1",
		"DFD2",
		"DFD3",
		"DFD4",
		"DFD5",
		"DFD6",
		"DFD7"
	};

	if (dev->path.type == DEVICE_PATH_PCI &&
	    PCI_SLOT(dev->path.pci.devfn) == DF_DEV)
		return df_acpi_names[PCI_FUNC(dev->path.pci.devfn)];

	printk(BIOS_ERR, "%s: Unhandled device id 0x%x\n", __func__, dev->device);
	return NULL;
}

struct device_operations mendocino_data_fabric_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= data_fabric_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};
