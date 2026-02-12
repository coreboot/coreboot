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

	/* Add opregion to host bridge needed for ACP driver.
	 *
	 * This is used by an ACPI method in the ACP's ACPI code to access different mailbox
	 * interfaces in the hardware. Some ACP drivers will use that to both notify the PSP
	 * that the DSP firmware has been loaded, so that the PSP can validate the firmware
	 * and set the qualifier bit to enable running it, and to configure the ACP's clock
	 * source.
	 *
	 * As this SMN access is not arbitrated and there may be other drivers or parts of
	 * the firmware attempting to use the SMN access register pair, there is a risk of
	 * conflict / incorrect data, but given the frequency and duration of accesses, the
	 * risk is deemed to be quite low.
	 */
	OperationRegion (PSMN, PCI_Config, 0x00, 0x100)
	Field(PSMN, AnyAcc, NoLock, Preserve) {
		Offset (0xB8),
		SMNA,   32,
		SMND,   32,
	}
}

/* PCIe GPP */
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
ACPI_PCI_DEV(GP17, 2, 7)

/* Internal GPP bridges */
ACPI_PCI_DEV(GP41, 8, 1)
ACPI_PCI_DEV(GP42, 8, 2)
ACPI_PCI_DEV(GP43, 8, 3)
