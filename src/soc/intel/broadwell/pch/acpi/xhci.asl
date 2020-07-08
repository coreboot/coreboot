/* SPDX-License-Identifier: GPL-2.0-only */

// XHCI Controller 0:14.0

Device (XHCI)
{
	Name (_ADR, 0x00140000)

	Name (PLSD, 5) // Port Link State - RxDetect
	Name (PLSP, 7) // Port Link State - Polling

	OperationRegion (XPRT, PCI_Config, 0, 0x100)
	Field (XPRT, AnyAcc, NoLock, Preserve)
	{
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
		OperationRegion (XREG, SystemMemory, ^XMEM << 16, 0x600)
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
		Name (PEDB, 1 << 1)

		// Change Status (Bits 23:17)
		Name (CHST, 0x7f << 17)

		// Port 0
		Local0 = PSC0 & ~PEDB
		PSC0 = Local0 | CHST

		// Port 1
		Local0 = PSC1 & ~PEDB
		PSC1 = Local0 | CHST

		// Port 2
		Local0 = PSC2 & ~PEDB
		PSC2 = Local0 | CHST

		// Port 3
		Local0 = PSC3 & ~PEDB
		PSC3 = Local0 | CHST
	}

	Method (LPS0, 0, Serialized)
	{
		OperationRegion (XREG, SystemMemory, ^XMEM << 16, 0x600)
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
		Local0 = 10
		While ((PPR1 == 1 && PLS1 == PLSP || PPR2 == 1 && PLS2 == PLSP) ||
		       (PPR3 == 1 && PLS3 == PLSP || PPR4 == 1 && PLS4 == PLSP))
		{
			If (Local0 == 0) {
				Break
			}
			Local0--
			Stall (10)
		}

		// For each USB3 Port:
		//   If port is disconnected (PLS=5 PP=1 CSC=0)
		//     1) Issue warm reset (WPR=1)
		//     2) Poll for warm reset complete (WRC=0)
		//     3) Write 1 to port status to clear

		// Local# indicate if port is reset
		Local1 = 0
		Local2 = 0
		Local3 = 0
		Local4 = 0

		If (PLS1 == PLSD && (CSC1 == 0 && PPR1 == 1)) {
			WPR1 = 1	// Issue warm reset
			Local1 = 1
		}
		If (PLS2 == PLSD && (CSC2 == 0 && PPR2 == 1)) {
			WPR2 = 1	// Issue warm reset
			Local2 = 1
		}
		If (PLS3 == PLSD && (CSC3 == 0 && PPR3 == 1)) {
			WPR3 = 1	// Issue warm reset
			Local3 = 1
		}
		If (PLS4 == PLSD && (CSC4 == 0 && PPR4 == 1)) {
			WPR4 = 1	// Issue warm reset
			Local4 = 1
		}

		// Poll for warm reset complete on all ports that were reset
		Local0 = 10
		While ((Local1 == 1 && WRC1 == 0 || Local2 == 1 && WRC2 == 0) ||
		       (Local3 == 1 && WRC3 == 0 || Local4 == 1 && WRC4 == 0))
		{
			If (Local0 == 0) {
				Break
			}
			Local0--
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
		If (^DVID == 0xFFFF) {
			Return ()
		}
		If (^XMEM == 0xFFFF || ^XMEM == 0) {
			Return ()
		}

		OperationRegion (XREG, SystemMemory, (^XMEM << 16) + 0x8000, 0x200)
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
		Local0 = ^D0D3
		if (Local0 == 3) {
			^D0D3 = 0
		}

		If (!\ISWP()) {
			// Clear PCI 0xB0[14:13]
			^MB13 = 0
			^MB14 = 0

			// Clear MMIO 0x816C[14,2]
			CLK0 = 0
			CLK1 = 0

			// Set MMIO 0x8154[31]
			CLK2 = 1

			// Handle per-port reset if needed
			LPS0 ()

			// Set MMIO 0x80e0[15]
			AX15 = 1

			// Clear PCI CFG offset 0x40[11]
			^SWAI = 0

			// Clear PCI CFG offset 0x44[13:12]
			^SAIP = 0
		}

		Return ()
	}

	Method (_PS3, 0, Serialized)
	{
		If (^DVID == 0xFFFF) {
			Return ()
		}
		If (^XMEM == 0xFFFF || ^XMEM == 0) {
			Return ()
		}

		OperationRegion (XREG, SystemMemory, (^XMEM << 16) + 0x8000, 0x200)
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

		^PMES = 1 // Clear PME Status
		^PMEE = 1 // Enable PME

		// If device is in D3, set back to D0
		Local0 = ^D0D3
		if (Local0 == 3) {
			^D0D3 = 0
		}

		If (!\ISWP()) {
			// Set PCI 0xB0[14:13]
			^MB13 = 1
			^MB14 = 1

			// Set MMIO 0x816C[14,2]
			CLK0 = 1
			CLK1 = 1

			// Clear MMIO 0x8154[31]
			CLK2 = 0

			// Clear MMIO 0x80e0[15]
			AX15 = 0

			// Set PCI CFG offset 0x40[11]
			^SWAI = 1

			// Set PCI CFG offset 0x44[13:12]
			^SAIP = 1
		}

		// Put device in D3
		^D0D3 = 3

		Return ()
	}

	Name (_PRW, Package (){ 0x6d, 3 })

	// Leave USB ports on for to allow Wake from USB

	Method (_S3D, 0)	// Highest D State in S3 State
	{
		Return (3)
	}

	Method (_S4D, 0)	// Highest D State in S4 State
	{
		Return (3)
	}

	Device (HUB7)
	{
		Name (_ADR, 0)

		// GPLD: Generate Port Location Data (PLD)
		Method (GPLD, 1, Serialized) {
			Name (PCKG, Package () {
				Buffer (0x10) {}
			})

			// REV: Revision 2 for ACPI 5.0
			CreateField (DerefOf (PCKG [0]), 0, 7, REV)
			REV = 2

			// VISI: Port visibility to user per port
			CreateField (DerefOf (PCKG [0]), 0x40, 1, VISI)
			VISI = Arg0
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
