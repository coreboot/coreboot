/* SPDX-License-Identifier: GPL-2.0-only */

// XHCI Controller 0:14.0

Device (XHCI)
{
	Name (_ADR, 0x00140000)

	Name (PLSD, 5) // Port Link State - RxDetect
	Name (PLSP, 7) // Port Link State - Polling

	OperationRegion (XPRT, PCI_Config, 0x00, 0x100)
	Field (XPRT, AnyAcc, NoLock, Preserve)
	{
		Offset (0x0),
		DVID, 16,
		Offset (0x10),
		, 16,
		XMEM, 16, // MEM_BASE
		Offset (0x40),
		, 11,
		SWAI, 1,
		, 20,
		Offset (0x44),
		, 12,
		SAIP, 2,
		, 18,
		Offset (0x74),
		D0D3, 2,
		, 6,
		PMEE, 1,  // PME_EN
		, 6,
		PMES, 1,  // PME_STS
		Offset (0xb0),
		, 13,
		MB13, 1,
		MB14, 1,
		Offset (0xd0),
		PR2R, 32,  // USB2PR
		PR2M, 32,  // USB2PRM
		PR3R, 32,  // USB3PR
		PR3M, 32,  // USB3PRM
	}

	// Clear status bits
	Method (LPCL, 0, Serialized)
	{
		OperationRegion (XREG, SystemMemory,
				 ShiftLeft (^XMEM, 16), 0x600)
		Field (XREG, DWordAcc, Lock, Preserve)
		{
			Offset (0x510), // PORTSCNUSB3[0]
			PSC0, 32,
			Offset (0x520), // PORTSCNUSB3[1]
			PSC1, 32,
			Offset (0x530), // PORTSCNUSB3[2]
			PSC2, 32,
			Offset (0x540), // PORTSCNUSB3[3]
			PSC3, 32,
		}

		// Port Enabled/Disabled (Bit 1)
		Name (PEDB, ShiftLeft (1, 1))

		// Change Status (Bits 23:17)
		Name (CHST, ShiftLeft (0x7f, 17))

		// Port 0
		And (PSC0, Not (PEDB), Local0)
		Or (Local0, CHST, PSC0)

		// Port 1
		And (PSC1, Not (PEDB), Local0)
		Or (Local0, CHST, PSC1)

		// Port 2
		And (PSC2, Not (PEDB), Local0)
		Or (Local0, CHST, PSC2)

		// Port 3
		And (PSC3, Not (PEDB), Local0)
		Or (Local0, CHST, PSC3)
	}

	Method (LPS0, 0, Serialized)
	{
		OperationRegion (XREG, SystemMemory,
				 ShiftLeft (^XMEM, 16), 0x600)
		Field (XREG, DWordAcc, Lock, Preserve)
		{
			Offset (0x510), // PORTSCNUSB3
			, 5,
			PLS1, 4,	// [8:5] Port Link State
			PPR1, 1,	// [9] Port Power
			, 7,
			CSC1, 1,	// [17] Connect Status Change
			, 1,
			WRC1, 1,	// [19] Warm Port Reset Change
			, 11,
			WPR1, 1,	// [31] Warm Port Reset
			Offset (0x520), // PORTSCNUSB3
			, 5,
			PLS2, 4,	// [8:5] Port Link State
			PPR2, 1,	// [9] Port Power
			, 7,
			CSC2, 1,	// [17] Connect Status Change
			, 1,
			WRC2, 1,	// [19] Warm Port Reset Change
			, 11,
			WPR2, 1,	// [31] Warm Port Reset
			Offset (0x530), // PORTSCNUSB3
			, 5,
			PLS3, 4,	// [8:5] Port Link State
			PPR3, 1,	// [9] Port Power
			, 7,
			CSC3, 1,	// [17] Connect Status Change
			, 1,
			WRC3, 1,	// [19] Warm Port Reset Change
			, 11,
			WPR3, 1,	// [31] Warm Port Reset
			Offset (0x540), // PORTSCNUSB3
			, 5,
			PLS4, 4,	// [8:5] Port Link State
			PPR4, 1,	// [9] Port Power
			, 7,
			CSC4, 1,	// [17] Connect Status Change
			, 1,
			WRC4, 1,	// [19] Warm Port Reset Change
			, 11,
			WPR4, 1,	// [31] Warm Port Reset
		}

		// Wait for all powered ports to finish polling
		Store (10, Local0)
		While (LOr (LOr (LAnd (LEqual (PPR1, 1), LEqual (PLS1, PLSP)),
				 LAnd (LEqual (PPR2, 1), LEqual (PLS2, PLSP))),
			    LOr (LAnd (LEqual (PPR3, 1), LEqual (PLS3, PLSP)),
				 LAnd (LEqual (PPR4, 1), LEqual (PLS4, PLSP)))))
		{
			If (LEqual (Local0, 0)) {
				Break
			}
			Decrement (Local0)
			Stall (10)
		}

		// For each USB3 Port:
		//   If port is disconnected (PLS=5 PP=1 CSC=0)
		//     1) Issue warm reset (WPR=1)
		//     2) Poll for warm reset complete (WRC=0)
		//     3) Write 1 to port status to clear

		// Local# indicate if port is reset
		Store (0, Local1)
		Store (0, Local2)
		Store (0, Local3)
		Store (0, Local4)

		If (LAnd (LEqual (PLS1, PLSD),
		          LAnd (LEqual (CSC1, 0), LEqual (PPR1, 1)))) {
			Store (1, WPR1)	      // Issue warm reset
			Store (1, Local1)
		}
		If (LAnd (LEqual (PLS2, PLSD),
		          LAnd (LEqual (CSC2, 0), LEqual (PPR2, 1)))) {
			Store (1, WPR2)	      // Issue warm reset
			Store (1, Local2)
		}
		If (LAnd (LEqual (PLS3, PLSD),
		          LAnd (LEqual (CSC3, 0), LEqual (PPR3, 1)))) {
			Store (1, WPR3)	      // Issue warm reset
			Store (1, Local3)
		}
		If (LAnd (LEqual (PLS4, PLSD),
		          LAnd (LEqual (CSC4, 0), LEqual (PPR4, 1)))) {
			Store (1, WPR4)	      // Issue warm reset
			Store (1, Local4)
		}

		// Poll for warm reset complete on all ports that were reset
		Store (10, Local0)
		While (LOr (LOr (LAnd (LEqual (Local1, 1), LEqual (WRC1, 0)),
				 LAnd (LEqual (Local2, 1), LEqual (WRC2, 0))),
			    LOr (LAnd (LEqual (Local3, 1), LEqual (WRC3, 0)),
			         LAnd (LEqual (Local4, 1), LEqual (WRC4, 0)))))
		{
			If (LEqual (Local0, 0)) {
				Break
			}
			Decrement (Local0)
			Stall (10)
		}

		// Clear status bits in all ports
		LPCL ()
	}

	Method (_PSC, 0, NotSerialized)
	{
		Return (^D0D3)
	}

	Method (_PS0, 0, Serialized)
	{
		If (LEqual (^DVID, 0xFFFF)) {
			Return ()
		}
		If (LOr (LEqual (^XMEM, 0xFFFF), LEqual (^XMEM, 0x0000))) {
			Return ()
		}

		OperationRegion (XREG, SystemMemory,
				 Add (ShiftLeft (^XMEM, 16), 0x8000), 0x200)
		Field (XREG, DWordAcc, Lock, Preserve)
		{
			Offset (0x0e0), // AUX Reset Control 1
			, 15,
			AX15, 1,
			Offset (0x154), // AUX Domain PM Control Register 2
			, 31,
			CLK2, 1,
			Offset (0x16c), // AUX Clock Control
			, 2,
			CLK0, 1,
			, 11,
			CLK1, 1, // USB3 Port Aux/Core Clock Gating Enable
		}

		// If device is in D3, set back to D0
		Store (^D0D3, Local0)
		if (LEqual (Local0, 3)) {
			Store (0, ^D0D3)
		}

		if (LNot (\ISWP())) {
			// Clear PCI 0xB0[14:13]
			Store (0, ^MB13)
			Store (0, ^MB14)

			// Clear MMIO 0x816C[14,2]
			Store (0, CLK0)
			Store (0, CLK1)

			// Set MMIO 0x8154[31]
			Store (1, CLK2)

			// Handle per-port reset if needed
			LPS0 ()

			// Set MMIO 0x80e0[15]
			Store (1, AX15)

			// Clear PCI CFG offset 0x40[11]
			Store (0, ^SWAI)

			// Clear PCI CFG offset 0x44[13:12]
			Store (0, ^SAIP)
		}

		Return ()
	}

	Method (_PS3, 0, Serialized)
	{
		If (LEqual (^DVID, 0xFFFF)) {
			Return ()
		}
		If (LOr (LEqual (^XMEM, 0xFFFF), LEqual (^XMEM, 0x0000))) {
			Return ()
		}

		OperationRegion (XREG, SystemMemory,
				 Add (ShiftLeft (^XMEM, 16), 0x8000), 0x200)
		Field (XREG, DWordAcc, Lock, Preserve)
		{
			Offset (0x0e0), // AUX Reset Control 1
			, 15,
			AX15, 1,
			Offset (0x154), // AUX Domain PM Control Register 2
			, 31,
			CLK2, 1,
			Offset (0x16c), // AUX Clock Control
			, 2,
			CLK0, 1,
			, 11,
			CLK1, 1, // USB3 Port Aux/Core Clock Gating Enable
		}

		Store (1, ^PMES) // Clear PME Status
		Store (1, ^PMEE) // Enable PME

		// If device is in D3, set back to D0
		Store (^D0D3, Local0)
		if (LEqual (Local0, 3)) {
			Store (0, ^D0D3)
		}

		if (LNot (\ISWP())) {
			// Set PCI 0xB0[14:13]
			Store (1, ^MB13)
			Store (1, ^MB14)

			// Set MMIO 0x816C[14,2]
			Store (1, CLK0)
			Store (1, CLK1)

			// Clear MMIO 0x8154[31]
			Store (0, CLK2)

			// Clear MMIO 0x80e0[15]
			Store (0, AX15)

			// Set PCI CFG offset 0x40[11]
			Store (1, ^SWAI)

			// Set PCI CFG offset 0x44[13:12]
			Store (1, ^SAIP)
		}

		// Put device in D3
		Store (3, ^D0D3)

		Return ()
	}

	Name (_PRW, Package(){ 0x6d, 3 })

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (3)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (3)
	}

	Device (HUB7)
	{
		Name (_ADR, 0x00000000)

		// GPLD: Generate Port Location Data (PLD)
		Method (GPLD, 1, Serialized) {
			Name (PCKG, Package (0x01) {
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

		Device (PRT1) { Name (_ADR, 1) } // USB Port 0
		Device (PRT2) { Name (_ADR, 2) } // USB Port 1
		Device (PRT3) { Name (_ADR, 3) } // USB Port 2
		Device (PRT4) { Name (_ADR, 4) } // USB Port 3
		Device (PRT5) { Name (_ADR, 5) } // USB Port 4
		Device (PRT6) { Name (_ADR, 6) } // USB Port 5
		Device (PRT7) { Name (_ADR, 7) } // USB Port 6
		Device (PRT8) { Name (_ADR, 8) } // USB Port 7
		Device (SSP1) { Name (_ADR, 10) } // USB Port 10
		Device (SSP2) { Name (_ADR, 11) } // USB Port 11
		Device (SSP3) { Name (_ADR, 12) } // USB Port 12
		Device (SSP4) { Name (_ADR, 13) } // USB Port 13
		Device (SSP5) { Name (_ADR, 14) } // USB Port 14
		Device (SSP6) { Name (_ADR, 15) } // USB Port 15
	}
}
