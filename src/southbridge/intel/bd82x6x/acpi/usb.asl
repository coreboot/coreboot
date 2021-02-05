/* SPDX-License-Identifier: GPL-2.0-only */

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
			CreateField (DerefOf (PCKG [0]), 0, 0x07, REV)
			REV = 0x02

			// VISI: Port visibility to user per port
			CreateField (DerefOf (PCKG [0]), 0x40, 1, VISI)
			VISI = Arg0

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
			CreateField (DerefOf (PCKG [0]), 0, 0x07, REV)
			REV = 0x02

			// VISI: Port visibility to user per port
			CreateField (DerefOf (PCKG [0]), 0x40, 1, VISI)
			VISI = Arg0

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
		If(Arg0 != 1) {
			// Set unknown revision bit
			CDW1 |= 8
		}

		// Set failure if xHCI is disabled by coreboot
		If(XHCI == 0) {
			CDW1 |= 2
		}

		// Query flag clear and xHCI in auto mode
		If(!(CDW1 & 0x1) && (XHCI == 2 || XHCI  == 3)) {
			Debug = "XHCI Switch"
			Local0 = 0
			Local0 = XPRT & 0x03
			If(Local0 == 0 || Local0 == 1) {
				Local1 = 0x0f
			}
			ElseIf(Local0 == 2) {
				Local1 = 3
			}
			ElseIf(Local0 == 3) {
				Local1 = 0
			}
			Local0 = RPM3 & 0xfffffff0
			RPM3 = Local0 | Local1
			Local0 = PRM2 & 0xfffffff0
			PRM2 = Local0 | Local1
			Local0 = SSEN & 0xfffffff0
			SSEN = Local0 | Local1
			Local0 = X2PR & 0xfffffff0
			X2PR = Local0 | Local1
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
