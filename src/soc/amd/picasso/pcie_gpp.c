/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <arch/ioapic.h>
#include <assert.h>
#include <amdblocks/amd_pci_util.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <stdio.h>

/* See AMD PPR 55570 - IOAPIC Initialization for the table that AGESA sets up */
const struct pci_routing_info pci_routing_table[] = {
	{PCIE_GPP_0_DEVFN, 0, PCI_SWIZZLE_ABCD, 0x10},
	{PCIE_GPP_1_DEVFN, 1, PCI_SWIZZLE_ABCD, 0x11},
	{PCIE_GPP_2_DEVFN, 2, PCI_SWIZZLE_ABCD, 0x12},
	{PCIE_GPP_3_DEVFN, 3, PCI_SWIZZLE_ABCD, 0x13},
	{PCIE_GPP_4_DEVFN, 4, PCI_SWIZZLE_ABCD, 0x10},
	{PCIE_GPP_5_DEVFN, 5, PCI_SWIZZLE_ABCD, 0x11},
	{PCIE_GPP_6_DEVFN, 6, PCI_SWIZZLE_ABCD, 0x12},
	{PCIE_GPP_A_DEVFN, 7, PCI_SWIZZLE_ABCD, 0x13},
	{PCIE_GPP_B_DEVFN, 7, PCI_SWIZZLE_CDAB, 0x0C},
};

const struct pci_routing_info *get_pci_routing_table(size_t *entries)
{
	*entries = ARRAY_SIZE(pci_routing_table);
	return pci_routing_table;
}

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

/*
 * This function writes a PCI device with _ADR, _STA, and _PRT objects:
 * Example:
 * Scope (\_SB.PCI0)
 * {
 *     Device (PBRA)
 *     {
 *         Name (_ADR, 0x0000000000080001)  // _ADR: Address
 *         Method (_STA, 0, NotSerialized)  // _STA: Status
 *         {
 *             Return (0x0F)
 *         }
 *
 *         Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
 *         {
 *             If (PICM)
 *             {
 *                 Return (Package (0x04)
 *                 {
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x00,
 *                         0x00,
 *                         0x00000034
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x01,
 *                         0x00,
 *                         0x00000035
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x02,
 *                         0x00,
 *                         0x00000036
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x03,
 *                         0x00,
 *                         0x00000037
 *                     }
 *                 })
 *             }
 *             Else
 *             {
 *                 Return (Package (0x04)
 *                 {
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x00,
 *                         \_SB.INTE,
 *                         0x00000000
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x01,
 *                         \_SB.INTF,
 *                         0x00000000
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x02,
 *                         \_SB.INTG,
 *                         0x00000000
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x03,
 *                         \_SB.INTH,
 *                         0x00000000
 *                     }
 *                 })
 *             }
 *         }
 *     }
 * }
 */
static void acpi_device_write_gpp_pci_dev(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);

	assert(dev->path.type == DEVICE_PATH_PCI);
	assert(name);
	assert(scope);

	acpigen_write_scope(scope);
	acpigen_write_device(name);

	acpigen_write_ADR_pci_device(dev);
	acpigen_write_STA(acpi_device_status(dev));

	acpigen_write_pci_GNB_PRT(dev);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}


static struct device_operations internal_pcie_gpp_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pci_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.acpi_name		= pcie_gpp_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_gpp_pci_dev,
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
	.acpi_fill_ssdt		= acpi_device_write_gpp_pci_dev,
};

static const struct pci_driver external_pcie_gpp_driver __pci_driver = {
	.ops			= &external_pcie_gpp_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.device			= PCI_DEVICE_ID_AMD_FAM17H_MODEL18H_PCIE_GPP,
};
