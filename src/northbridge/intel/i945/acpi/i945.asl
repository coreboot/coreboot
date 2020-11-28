/* SPDX-License-Identifier: GPL-2.0-only */

#include "hostbridge.asl"
#include "../i945.h"
#include <southbridge/intel/common/rcba.h>

/* Operating System Capabilities Method */
Method (_OSC, 4)
{
	/* Check for proper PCI/PCIe UUID */
	If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
	{
		/* Let OS control everything */
		Return(Arg3)
	} Else {
		CreateDWordField(Arg3, 0, CDW1)
		CDW1 = CDW1 | 4	// Unrecognized UUID, so set bit 2 to 1
		Return(Arg3)
	}
}

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	// This does not seem to work correctly yet - set values statically for
	// now.

	//Name (PDRS, ResourceTemplate() {
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00004000, RCRB) // RCBA
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00004000, MCHB) // MCHBAR
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00001000, DMIB) // DMIBAR
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00001000, EGPB) // EPBAR
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00000000, PCIE) // PCIE BAR
	//	Memory32Fixed(ReadWrite, 0xfed20000, 0x00070000, ICHB) // Misc ICH
	//})

	Name (PDRS, ResourceTemplate() {
		Memory32Fixed(ReadWrite, DEFAULT_RCBA, 0x00004000)
		Memory32Fixed(ReadWrite, DEFAULT_MCHBAR, 0x00004000)
		Memory32Fixed(ReadWrite, DEFAULT_DMIBAR, 0x00001000)
		Memory32Fixed(ReadWrite, DEFAULT_EPBAR, 0x00001000)
		Memory32Fixed(ReadWrite, CONFIG_MMCONF_BASE_ADDRESS, 0x04000000)
		Memory32Fixed(ReadWrite, 0xfed20000, 0x00020000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed40000, 0x00005000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed45000, 0x0004b000) // Misc ICH
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		//CreateDwordField(PDRS, ^RCRB._BAS, RBR0)
		//RBR0 = \_SB.PCI0.LPCB.RCBA << 14

		//CreateDwordField(PDRS, ^MCHB._BAS, MBR0)
		//MBR0 = \_SB.PCI0.MCHC.MHBR << 14

		//CreateDwordField(PDRS, ^DMIB._BAS, DBR0)
		//DBR0 = \_SB.PCI0.MCHC.DMBR << 12

		//CreateDwordField(PDRS, ^EGPB._BAS, EBR0)
		//EBR0 = \_SB.PCI0.MCHC.EPBR << 12

		//CreateDwordField(PDRS, ^PCIE._BAS, PBR0)
		//PBR0 = \_SB.PCI0.MCHC.PXBR << 26

		//CreateDwordField(PDRS, ^PCIE._LEN, PSZ0)
		//PSZ0 = 0x10000000 << \_SB.PCI0.MCHC.PXSZ

		Return(PDRS)
	}
}

// PCIe graphics port 0:1.0
#include "peg.asl"

// Integrated graphics 0:2.0
#include <drivers/intel/gma/acpi/gfx.asl>
#include "igd.asl"
