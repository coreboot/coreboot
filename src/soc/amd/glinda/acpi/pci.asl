/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/alib.h>
#include <amdblocks/gpio_defs.h>
#include <amdblocks/xhci.h>
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

/* USB4.0 bridge */
ACPI_PCI_DEV(GP09, 1, 1)
ACPI_PCI_DEV(GP0A, 1, 2)

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
Scope (\_SB.PCI0.GP41)
{
	/* IGFX is currently emmited in SSDT */

	Device (HDAU)
	{
		Name (_ADR, 0x01) /* Device 0, Function 1 */
		Name (_S0W, 4)
	}

	Device (PSP)
	{
		Name (_ADR, 0x02) /* Device 0, Function 2 */
	}

	/* Device (XHC1) is emitted in SSDT */

	Device (ACPD)
	{
		Name (_ADR, 0x05) /* Device 0, Function 5 */
		Name (_S0W, 4)
	}

	Device (AZAL)
	{
		Name(_ADR, 0x06) /* Device 0, Function 6 */
		Name (_S0W, 4)
	}

	Device (MP2C)
	{
		Name (_ADR, 0x07) /* Device 0, Function 7 */
	}
}

ACPI_PCI_DEV(GP42, 8, 2)
Scope (\_SB.PCI0.GP42)
{
	Device (IPU)
	{
		Name(_ADR, 0x01) /* Device 0, Function 1 */
		Name (_S0W, 4)
	}
}

ACPI_PCI_DEV(GP43, 8, 3)
Scope (\_SB.PCI0.GP43)
{
	/* Device (XHC0) is emitted in SSDT */
	/* Device (XHC3) is emitted in SSDT */
	/* Device (XHC4) is emitted in SSDT */

	Device (NHI0)
	{
		Name(_ADR, 0x05) /* Device 0, Function 5 */
		Name (_S0W, 4)
		Method (_RST, 0, Serialized)
		{
			\_SB.ALIB (ALIB_FUNCTION_USB4_ROUTER_HOST_RESET, Zero)
		}
		Name (_PRW, Package (0x02) {XHCI_GEVENT, 0x04})
	}

	Device (NHI1)
	{
		Name(_ADR, 0x06) /* Device 0, Function 6 */
		Name (_S0W, 4)
		Method (_RST, 0, Serialized)
		{
			\_SB.ALIB (ALIB_FUNCTION_USB4_ROUTER_HOST_RESET, One)
		}
		Name (_PRW, Package (0x02) {XHCI_GEVENT, 0x04})
	}
}
