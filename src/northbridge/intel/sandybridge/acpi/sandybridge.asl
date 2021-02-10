/* SPDX-License-Identifier: GPL-2.0-only */

#include "hostbridge.asl"
#include "peg.asl"

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	Name (PDRS, ResourceTemplate() {
		Memory32Fixed(ReadWrite, CONFIG_FIXED_RCBA_MMIO_BASE, CONFIG_RCBA_LENGTH)
		// Filled by _CRS
		Memory32Fixed(ReadWrite, 0, 0x00008000, MCHB)
		Memory32Fixed(ReadWrite, 0, 0x00001000, DMIB)
		Memory32Fixed(ReadWrite, 0, 0x00001000, EGPB)
		Memory32Fixed(ReadWrite, 0, 0x04000000, PCIX)
		Memory32Fixed(ReadWrite, 0xfed20000, 0x00020000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed40000, 0x00005000) // TPM TIS
		Memory32Fixed(ReadWrite, 0xfed45000, 0x0004b000) // Misc ICH

		/* Required for SandyBridge sighting 3715511 */
		Memory32Fixed(ReadWrite, 0x20000000, 0x00200000)
		Memory32Fixed(ReadWrite, 0x40000000, 0x00200000)
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		CreateDwordField (PDRS, ^MCHB._BAS, MBR0)
		MBR0 = \_SB.PCI0.MCHC.MHBR << 15

		CreateDwordField (PDRS, ^DMIB._BAS, DBR0)
		DBR0 = \_SB.PCI0.MCHC.DMBR << 12

		CreateDwordField (PDRS, ^EGPB._BAS, EBR0)
		EBR0 = \_SB.PCI0.MCHC.EPBR << 12

		CreateDwordField (PDRS, ^PCIX._BAS, XBR0)
		XBR0 = \_SB.PCI0.MCHC.PXBR << 26

		CreateDwordField (PDRS, ^PCIX._LEN, XSZ0)
		XSZ0 = 0x10000000 >> \_SB.PCI0.MCHC.PXSZ

		Return(PDRS)
	}
}

/* Integrated graphics 0:2.0 */
#include <drivers/intel/gma/acpi/gfx.asl>
