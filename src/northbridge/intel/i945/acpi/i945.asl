/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "hostbridge.asl"
#include "../i945.h"

/* Operating System Capabilities Method */
Method (_OSC, 4)
{
	/* Check for proper PCI/PCIe UUID */
	If (LEqual(Arg0, ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")))
	{
		/* Let OS control everything */
		Return(Arg3)
	} Else {
		CreateDWordField(Arg3, 0, CDW1)
		Or(CDW1, 4, CDW1)	// Unrecognized UUID, so set bit 2 to 1
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
		Memory32Fixed(ReadWrite, 0xfed1c000, 0x00004000) // RCBA
		Memory32Fixed(ReadWrite, DEFAULT_MCHBAR,   0x00004000)
		Memory32Fixed(ReadWrite, DEFAULT_DMIBAR,   0x00001000)
		Memory32Fixed(ReadWrite, DEFAULT_EPBAR,    0x00001000)
		Memory32Fixed(ReadWrite, DEFAULT_PCIEXBAR, 0x04000000)
		Memory32Fixed(ReadWrite, 0xfed20000, 0x00020000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed40000, 0x00005000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed45000, 0x0004b000) // Misc ICH
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		//CreateDwordField(PDRS, ^RCRB._BAS, RBR0)
		//ShiftLeft(\_SB.PCI0.LPCB.RCBA, 14, RBR0)

		//CreateDwordField(PDRS, ^MCHB._BAS, MBR0)
		//ShiftLeft(\_SB.PCI0.MCHC.MHBR, 14, MBR0)

		//CreateDwordField(PDRS, ^DMIB._BAS, DBR0)
		//ShiftLeft(\_SB.PCI0.MCHC.DMBR, 12, DBR0)

		//CreateDwordField(PDRS, ^EGPB._BAS, EBR0)
		//ShiftLeft(\_SB.PCI0.MCHC.EPBR, 12, EBR0)

		//CreateDwordField(PDRS, ^PCIE._BAS, PBR0)
		//ShiftLeft(\_SB.PCI0.MCHC.PXBR, 26, PBR0)

		//CreateDwordField(PDRS, ^PCIE._LEN, PSZ0)
		//ShiftLeft(0x10000000, \_SB.PCI0.MCHC.PXSZ, PSZ0)

		Return(PDRS)
	}
}

// PCIe graphics port 0:1.0
#include "peg.asl"

// Integrated graphics 0:2.0
#include "igd.asl"
