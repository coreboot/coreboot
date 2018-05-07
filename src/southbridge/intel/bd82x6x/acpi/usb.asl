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

/* Intel Cougar Point USB support */

// EHCI Controller 0:1d.0

Device (EHC1)
{
	Name(_ADR, 0x001d0000)

	Name (_PRW, Package(){ 13, 4 }) // Power Resources for Wake

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}

	Device (HUB7)
	{
		Name (_ADR, 0x00000000)


		// GPLD: Generate Port Location Data (PLD)
		Method (GPLD, 1, Serialized)
		 {

			Name (PCKG, Package (0x01)
			{
				Buffer (0x10) {}
			})

			// REV: Revision 0x02 for ACPI 5.0
			CreateField (DerefOf (Index (PCKG, Zero)), Zero, 0x07, REV)
			Store (0x02, REV)

			// VISI: Port visibility to user per port
			CreateField (DerefOf (Index (PCKG, Zero)), 0x40, One, VISI)
			Store (Arg0, VISI)

			Return (PCKG)
                }

		// How many are there?
		Device (PRT1) { Name (_ADR, 1) } // USB Port 0
		Device (PRT2) { Name (_ADR, 2) } // USB Port 1
		Device (PRT3) { Name (_ADR, 3) } // USB Port 2
		Device (PRT4) { Name (_ADR, 4) } // USB Port 3
		Device (PRT5) { Name (_ADR, 5) } // USB Port 4
		Device (PRT6) { Name (_ADR, 6) } // USB Port 5
	}
}

// EHCI #2 Controller 0:1a.0

Device (EHC2)
{
	Name(_ADR, 0x001a0000)

	Name (_PRW, Package(){ 13, 4 }) // Power Resources for Wake

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}

	Device (HUB7)
	{
		Name (_ADR, 0x00000000)

		// GPLD: Generate Port Location Data (PLD)
		Method (GPLD, 1, Serialized)
		 {
			Name (PCKG, Package (0x01)
			{
				Buffer (0x10) {}
			})

			// REV: Revision 0x02 for ACPI 5.0
			CreateField (DerefOf (Index (PCKG, Zero)), Zero, 0x07, REV)
			Store (0x02, REV)

			// VISI: Port visibility to user per port
			CreateField (DerefOf (Index (PCKG, Zero)), 0x40, One, VISI)
			Store (Arg0, VISI)

			Return (PCKG)
                }

		// How many are there?
		Device (PRT1) { Name (_ADR, 1) } // USB Port 0
		Device (PRT2) { Name (_ADR, 2) } // USB Port 1
		Device (PRT3) { Name (_ADR, 3) } // USB Port 2
		Device (PRT4) { Name (_ADR, 4) } // USB Port 3
		Device (PRT5) { Name (_ADR, 5) } // USB Port 4
		Device (PRT6) { Name (_ADR, 6) } // USB Port 5
	}
}

Device (XHC)
{
	Name(_ADR, 0x00140000)
	OperationRegion(XDEV, PCI_Config, 0, 256)
	Field(XDEV, DWordAcc, NoLock, Preserve)
	{
		Offset(0xD0),
		X2PR, 32,	// XUSB2PR
		PRM2, 32,	// XUSB2PRM
		SSEN, 32,	// USB3_PSSEN
		RPM3, 32,	// USB3PRM
		XPRT, 32,	// XHCI Ports
	}

	Name (_PRW, Package(){ 13, 4 }) // Power Resources for Wake

	Method(POSC,2,Serialized)
	{
	/*
	 * Arg0 - An Integer containing a count of entries in Arg3
	 * Arg1 - A Buffer containing a list of DWORD capabilities
	 */

		// Create DWord field from the Capabilities Buffer
		CreateDWordField(Arg1,0,CDW1)

		// Check revision
		If(LNotEqual(Arg0,One)) {
			// Set unknown revision bit
			Or(CDW1,0x8,CDW1)
		}

		// Set failure if xHCI is disabled by coreboot
		If(LEqual(XHCI, 0)) {
			Or(CDW1,0x2,CDW1)
		}

		// Query flag clear and xHCI in auto mode
		If(LAnd(LNot(And(CDW1,0x1)),LOr(LEqual(XHCI ,2), LEqual(XHCI ,3)))) {
			Store ("XHCI Switch", Debug)
			Store(Zero, Local0)
			And(XPRT, 0x3, Local0)
			If(LOr(LEqual(Local0, 0), LEqual(Local0, 1))) {
				Store(0xF, Local1)
			}
			ElseIf(LEqual(Local0, 2)) {
				Store(0x3, Local1)
			}
			ElseIf(LEqual(Local0, 3)) {
				Store(Zero, Local1)
			}
			And(RPM3, 0xFFFFFFF0, Local0)
			Or(Local0, Local1, RPM3)
			And(PRM2, 0xFFFFFFF0, Local0)
			Or(Local0, Local1, PRM2)
			And(SSEN, 0xFFFFFFF0, Local0)
			Or(Local0, Local1, SSEN)
			And(X2PR, 0xFFFFFFF0, Local0)
			Or(Local0, Local1, X2PR)
		}
		Return(Arg1)
	}

	// Leave USB ports on for to allow Wake from USB
	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}
}
