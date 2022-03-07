/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static const char *cnvi_wifi_acpi_name(const struct device *dev)
{
	return "CNVW";
}

static struct device_operations cnvi_wifi_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus,
	.acpi_name		= cnvi_wifi_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static const unsigned short wifi_pci_device_ids[] = {
	PCI_DID_INTEL_CML_LP_CNVI_WIFI,
	PCI_DID_INTEL_CML_H_CNVI_WIFI,
	PCI_DID_INTEL_CNL_LP_CNVI_WIFI,
	PCI_DID_INTEL_CNL_H_CNVI_WIFI,
	PCI_DID_INTEL_GLK_CNVI_WIFI,
	PCI_DID_INTEL_ICL_CNVI_WIFI,
	PCI_DID_INTEL_JSL_CNVI_WIFI_0,
	PCI_DID_INTEL_JSL_CNVI_WIFI_1,
	PCI_DID_INTEL_JSL_CNVI_WIFI_2,
	PCI_DID_INTEL_JSL_CNVI_WIFI_3,
	PCI_DID_INTEL_TGL_CNVI_WIFI_0,
	PCI_DID_INTEL_TGL_CNVI_WIFI_1,
	PCI_DID_INTEL_TGL_CNVI_WIFI_2,
	PCI_DID_INTEL_TGL_CNVI_WIFI_3,
	PCI_DID_INTEL_TGL_H_CNVI_WIFI_0,
	PCI_DID_INTEL_TGL_H_CNVI_WIFI_1,
	PCI_DID_INTEL_TGL_H_CNVI_WIFI_2,
	PCI_DID_INTEL_TGL_H_CNVI_WIFI_3,
	PCI_DID_INTEL_ADL_N_CNVI_WIFI_0,
	PCI_DID_INTEL_ADL_N_CNVI_WIFI_1,
	PCI_DID_INTEL_ADL_N_CNVI_WIFI_2,
	PCI_DID_INTEL_ADL_N_CNVI_WIFI_3,
	0
};

static const struct pci_driver pch_cnvi_wifi __pci_driver = {
	.ops			= &cnvi_wifi_ops,
	.vendor			= PCI_VID_INTEL,
	.devices		= wifi_pci_device_ids,
};

static const char *cnvi_bt_acpi_name(const struct device *dev)
{
	return "CNVB";
}

static struct device_operations cnvi_bt_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus,
	.acpi_name		= cnvi_bt_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static const unsigned short bt_pci_device_ids[] = {
	PCI_DID_INTEL_TGL_CNVI_BT_0,
	PCI_DID_INTEL_TGL_CNVI_BT_1,
	PCI_DID_INTEL_TGL_CNVI_BT_2,
	PCI_DID_INTEL_TGL_CNVI_BT_3,
	PCI_DID_INTEL_TGL_H_CNVI_BT_0,
	PCI_DID_INTEL_TGL_H_CNVI_BT_1,
	PCI_DID_INTEL_TGL_H_CNVI_BT_2,
	0
};

static const struct pci_driver pch_cnvi_bt __pci_driver = {
	.ops			= &cnvi_bt_ops,
	.vendor			= PCI_VID_INTEL,
	.devices		= bt_pci_device_ids,
};
