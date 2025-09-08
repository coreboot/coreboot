/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/common/acpi/pci_template.asl>

/*
 * Keep name in sync with SSDT generator pcie_gpp_acpi_name()!
 * Currently the name is hex representation of dev->path.pci.devfn.
 */

/* USB4.0 bridge */
ACPI_PCI_DEV(GP09, 1, 1)
ACPI_PCI_DEV(GP0A, 1, 2)
ACPI_PCI_DEV(GP0B, 1, 3)

/* PCIe GPP */
ACPI_PCI_DEV(GP11, 2, 1)
ACPI_PCI_DEV(GP12, 2, 2)
ACPI_PCI_DEV(GP13, 2, 3)
ACPI_PCI_DEV(GP14, 2, 4)
ACPI_PCI_DEV(GP15, 2, 5)
ACPI_PCI_DEV(GP16, 2, 6)

/* PCIe GPP */
ACPI_PCI_DEV(GP19, 3, 1)
ACPI_PCI_DEV(GP1A, 3, 2)
ACPI_PCI_DEV(GP1B, 3, 3)
ACPI_PCI_DEV(GP1C, 3, 4)
ACPI_PCI_DEV(GP1D, 3, 5)
ACPI_PCI_DEV(GP1E, 3, 6)

/* Internal GPP bridges */
ACPI_PCI_DEV(GP41, 8, 1)
ACPI_PCI_DEV(GP42, 8, 2)
ACPI_PCI_DEV(GP43, 8, 3)
