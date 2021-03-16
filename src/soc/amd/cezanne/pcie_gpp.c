/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pciexp.h>
#include <soc/pci_devs.h>

static const char *pcie_gpp_acpi_name(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCIE_GPP_1_0_DEVFN:
		return "GP10";
	case PCIE_GPP_1_1_DEVFN:
		return "GP11";
	case PCIE_GPP_1_2_DEVFN:
		return "GP12";
	case PCIE_GPP_2_0_DEVFN:
		return "GP20";
	case PCIE_GPP_2_1_DEVFN:
		return "GP21";
	case PCIE_GPP_2_2_DEVFN:
		return "GP22";
	case PCIE_GPP_2_3_DEVFN:
		return "GP23";
	case PCIE_GPP_2_4_DEVFN:
		return "GP24";
	case PCIE_GPP_2_5_DEVFN:
		return "GP25";
	case PCIE_GPP_2_6_DEVFN:
		return "GP26";
	case PCIE_ABC_A_DEVFN:
		return "GPPA";
	case PCIE_GPP_B_DEVFN:
		return "GPPB";
	case PCIE_GPP_C_DEVFN:
		return "GPPC";
	default:
		printk(BIOS_ERR, "%s: Unhanded devfn 0x%x\n", __func__, dev->path.pci.devfn);
		return NULL;
	}
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

static const struct pci_driver internal_pcie_gpp_driver __pci_driver = {
	.ops			= &internal_pcie_gpp_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.device			= PCI_DEVICE_ID_AMD_FAM17H_MODEL60H_PCIE_GPP_BUSABC,
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

static const unsigned short external_pci_gpp_ids[] = {
	PCI_DEVICE_ID_AMD_FAM17H_MODEL60H_PCIE_GPP_D1,
	PCI_DEVICE_ID_AMD_FAM17H_MODEL60H_PCIE_GPP_D2,
	0
};

static const struct pci_driver external_pcie_gpp_driver __pci_driver = {
	.ops			= &external_pcie_gpp_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.devices		= external_pci_gpp_ids,
};
