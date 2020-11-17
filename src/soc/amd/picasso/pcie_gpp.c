/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <stdio.h>

static const char *pcie_gpp_acpi_name(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCIE_GPP_0_DEVFN:
		return "PBR0";
	case PCIE_GPP_1_DEVFN:
		return "PBR1";
	case PCIE_GPP_2_DEVFN:
		return "PBR2";
	case PCIE_GPP_3_DEVFN:
		return "PBR3";
	case PCIE_GPP_4_DEVFN:
		return "PBR4";
	case PCIE_GPP_5_DEVFN:
		return "PBR5";
	case PCIE_GPP_6_DEVFN:
		return "PBR6";
	case PCIE_GPP_A_DEVFN:
		return "PBRA";
	case PCIE_GPP_B_DEVFN:
		return "PBRB";
	}

	return NULL;
}

static struct device_operations internal_pcie_gpp_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pci_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.acpi_name		= pcie_gpp_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_FAM17H_MODEL18H_PCIE_GPP_BUSA,
	PCI_DEVICE_ID_AMD_FAM17H_MODEL18H_PCIE_GPP_BUSB,
	0
};

static const struct pci_driver internal_pcie_gpp_driver __pci_driver = {
	.ops			= &internal_pcie_gpp_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.devices		= pci_device_ids,
};

static struct device_operations external_pcie_gpp_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.acpi_name		= pcie_gpp_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static const struct pci_driver external_pcie_gpp_driver __pci_driver = {
	.ops			= &external_pcie_gpp_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.device			= PCI_DEVICE_ID_AMD_FAM17H_MODEL18H_PCIE_GPP,
};
