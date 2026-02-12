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
ACPI_PCI_DEV(GP09, 1, 1)
ACPI_PCI_DEV(GP0A, 1, 2)
ACPI_PCI_DEV(GP0B, 1, 3)
ACPI_PCI_DEV(GP0C, 1, 4)
ACPI_PCI_DEV(GP0D, 1, 5)
ACPI_PCI_DEV(GP0E, 1, 6)
ACPI_PCI_DEV(GP0F, 1, 7)

/* Internal GPP bridges */
ACPI_PCI_DEV(GP41, 8, 1)
ACPI_PCI_DEV(GP42, 8, 2)
ACPI_PCI_DEV(GP43, 8, 3)
