/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\_SB) {

	Name(NIMH, "NiMH")
	Name(LION, "Lion")

	Device (BAT0)
	{
		Name(_HID, EisaId("PNP0C0A"))
		Name(_UID, 1)

		Name(_PCL, Package(){ \_SB })

		Name(PBST, Package() { 0x00, 0x04b0, 0x0bb8, 0x03e8 })

		/* Status */
		Method(_STA, 0)
		{
			Sleep(120)
			If (\_SB.PCI0.LPCB.EC0.BAT) {
				Return (0x1f)
			} Else {
				Return (0x0f)
			}
		}

		/* Battery Info */
		Method(_BIF, 0, Serialized)
		{
			Name(PBIF, Package() {
				0x01,
				0x0fa0,
				0x0fa0,
				0x01,
				0x2ee0,	// Capacity?
				0x012c,
				0x96,
				0x01,
				0x01,
				"Battery",	// Name
				"",		// Number
				"LIon",		// Type
				"Generic"	// Vendor
			})

			// Is battery there?
			Local0 = _STA ()
			Local0 &= 0x10
			If (!Local0) {
				Return (PBIF)
			}

			PBIF [1] = \_SB.PCI0.LPCB.EC0.BDC0
			PBIF [4] = \_SB.PCI0.LPCB.EC0.BDV0

			Local0 = \_SB.PCI0.LPCB.EC0.BFC0
			PBIF [2] = Local0

			Local2 = Local0 / 10
			PBIF [5] = Local2

			Local2 = Local0 / 20
			PBIF [6] = Local2

			If (\_SB.PCI0.LPCB.EC0.BTYP) {
				PBIF [11] = NIMH
			} Else {
				PBIF [11] = LION
			}

			Return (PBIF)
		}

		/* Battery Status */
		Method(_BST, 0)
		{
			If (\_SB.PCI0.LPCB.EC0.BAT) {
				PBST [3] = \_SB.PCI0.LPCB.EC0.BPV0

				Local3 = \_SB.PCI0.LPCB.EC0.BRC0 * 100
				Local0 = Local3 / \_SB.PCI0.LPCB.EC0.BFC0
				Local3 = \_SB.PCI0.LPCB.EC0.BFC0 * Local0
				Local0 = Local3 / 100
				Local0++
				PBST [2] = Local0

				Local3 = \_SB.PCI0.LPCB.EC0.BRC0
				Local0 = \_SB.PCI0.LPCB.EC0.BPR0
				Local0 = ~Local0 & 0xFFFF
				PBST [1] = Local0

				// AC Power connected?
				If (\_SB.PCI0.LPCB.EC0.ADP) {
					If (\_SB.PCI0.LPCB.EC0.CHRG) {
						PBST [0] = 2
					} Else {
						PBST [0] = 0
					}
				} Else {
					If (Local3 < 25) {
						PBST [0] = 5
					} Else {
						PBST [0] = 1
					}
				}
			}
			Return (PBST)
		}
	}


	Device (BAT1)
	{
		Name(_HID, EisaId("PNP0C0A"))
		Name(_UID, 1)

		Name(_PCL, Package(){ \_SB })

		Name(PBST, Package() { 0x00, 0x04b0, 0x0bb8, 0x03e8 })

		/* Status */
		Method(_STA, 0)
		{
			Sleep(120)
			If (\_SB.PCI0.LPCB.EC0.BAT2) {
				Return (0x1f)
			} Else {
				Return (0x0f)
			}
		}

		/* Battery Info */
		Method(_BIF, 0, Serialized)
		{
			Name(PBIF, Package() {
				0x01,
				0x0fa0,
				0x0fa0,
				0x01,
				0x2ee0,	// Capacity?
				0x012c,
				0x96,
				0x01,
				0x01,
				"Battery",	// Name
				"",		// Number
				"LIon",		// Type
				"Generic"	// Vendor
			})

			// Is battery there?
			Local0 = _STA ()
			Local0 &= 0x10
			If (!Local0) {
				Return (PBIF)
			}

			PBIF [1] = \_SB.PCI0.LPCB.EC0.BDC2
			PBIF [4] = \_SB.PCI0.LPCB.EC0.BDV2

			Local0 = \_SB.PCI0.LPCB.EC0.BFC2
			PBIF [2] = Local0

			Local2 = Local0 / 10
			PBIF [5] = Local2

			Local2 = Local0 / 20
			PBIF [6] = Local2

			If (\_SB.PCI0.LPCB.EC0.BTY2) {
				PBIF [11] = NIMH
			} Else {
				PBIF [11] = LION
			}

			Return (PBIF)
		}

		/* Battery Status */
		Method(_BST, 0)
		{
			If (\_SB.PCI0.LPCB.EC0.BAT2) {
				PBST [3] = \_SB.PCI0.LPCB.EC0.BPV2

				Local3 = \_SB.PCI0.LPCB.EC0.BRC2 * 100
				Local0 = Local3 / \_SB.PCI0.LPCB.EC0.BRC2
				Local3 = \_SB.PCI0.LPCB.EC0.BFC2 * Local0
				Local0 = Local3 / 100
				Local0++
				PBST [2] = Local0

				Local3 = \_SB.PCI0.LPCB.EC0.BRC2
				Local0 = \_SB.PCI0.LPCB.EC0.BPR2
				Local0 = ~Local0 & 0xFFFF
				PBST [1] = Local0

				// AC Power connected?
				If (\_SB.PCI0.LPCB.EC0.ADP) {
					If (\_SB.PCI0.LPCB.EC0.CRG2) {
						PBST [0] = 2
					} Else {
						PBST [0] = 0
					}
				} Else {
					If (Local3 < 25) {
						PBST [0] = 5
					} Else {
						PBST [0] = 1
					}
				}
			}
			Return (PBST)
		}
	}

	Name (PWRS, 0)
	Name (ACFG, 0)

	Device (AC)
	{
		Name (_HID, "ACPI0003")
		Name (ACST, 0x00)
		Method (_PSR, 0)
		{
			If (ACFG) {
				Local0 = ACST
			} Else {
				Local0 = \_SB.PCI0.LPCB.EC0.ADP
				ACST = Local0
				ACFG = 1
			}
			Sleep(120)
			Return (Local0)
		}

		Name(_PCL, Package(){
			\_SB,
			BAT0,
			BAT1
		})
	}
}
