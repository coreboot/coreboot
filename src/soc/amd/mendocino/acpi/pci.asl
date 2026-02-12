/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/common/acpi/pci_template.asl>

/*
 * Keep name in sync with SSDT generator pcie_gpp_acpi_name()!
 * Currently the name is hex representation of dev->path.pci.devfn.
 */

/* Root complex */
Device (GNB)
{
	Name (_ADR, 0)
	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}
}

/* PCIe GPP */
ACPI_PCI_DEV(GP11, 2, 1)
ACPI_PCI_DEV(GP12, 2, 2)
ACPI_PCI_DEV(GP13, 2, 3)
ACPI_PCI_DEV(GP14, 2, 4)
ACPI_PCI_DEV(GP15, 2, 5)
ACPI_PCI_DEV(GP16, 2, 6)

/* Internal GPP bridges */
ACPI_PCI_DEV(GP41, 8, 1)
ACPI_PCI_DEV(GP42, 8, 2)
ACPI_PCI_DEV(GP43, 8, 3)
