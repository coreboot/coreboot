/* SPDX-License-Identifier: GPL-2.0-only */

Name(\CBA1, 0x60)
Name(\CBA2, 0x60)

Device (BAT1)
{
	Name(_HID, EisaId("PNP0C0A"))
	Name(_UID, 1)

	Name(_PCL, Package(){ _SB })
	Name(PSTA, 0x1f)
	Name(PBIF, Package() {
		0x00,
		0x78,
		0x64,
		0x01,
		0x2b5c,	// Capacity?
		0x05,
		0x03,
		0x01,
		0x01,
		"???",	// Name
		"???",	// Number
		"???",	// Type
		"???"	// Vendor
	})
	Name(PBST, Package() { 0x01, 0x16, 0x64, 0x2b5c })

	/* Status */
	Method(_STA, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			If(\_SB.PCI0.LPCB.EC0.P62S) {
				Return(0x0f)
			} Else {
				Return(0x1f)
			}
		} Else {
			Return(0x0f)
		}
	}

	/* Battery Info */
	Method(_BIF, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			If(\_SB.PCI0.LPCB.EC0.P62S) {
				IVBI()
				IVBS()
			} Else {
				UPBI()
			}
		}

		Return(PBIF)
	}

	/* Battery Status */
	Method(_BST, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			UPBS()
		}

		Return(PBST)
	}

	/* Update Battery Info */
	Method(UPBI, 0)
	{
		PBIF [1] = 0x78
		PBIF [2] = 0x64
		PBIF [4] = 0x2b5c
		PBIF [9] = "Bat1"
		PBIF [10] = "001"
		PBIF [11] = "LION"
		PBIF [12] = "Panasonic"
	}

	Method(UPBS, 0)
	{
		Local0 = \_SB.PCI0.LPCB.EC0.QEVT
		If (~Local0) {
			GP38 = 0
			Sleep(0x64)
			Local0 = GP38
			If (~Local0) {
				Local0 = RDW (0x0d)
				If (Local0 != 0xeeee) {
					If (Local0 <= 0x64) {
						CBA1 = Local0
					}
				}
			}
		}

		Local0 = CBA1
		PBST [2] = Local0
		PBST [3] = DerefOf (PBIF [4])
		Local1 = 0

		If (PWRS) {
			If (Local0 < 0x64) {
				Local1 = 2
			}
		} Else {
			If (Local0 <= 0x5) {
				Local1 = 4
			} Else {
				Local1 = 1
			}
		}

		PBST [0] = Local1
		If (\_SB.PCI0.LPCB.EC0.P63S) {
			PBST [1] = 0x16
		} Else  {
			PBST [1] = 0x0b
		}
	}

	// Invalidate Battery Info
	Method(IVBI, 0)
	{
		PBIF [1] = 0xffffffff
		PBIF [2] = 0xffffffff
		PBIF [4] = 0xffffffff
		PBIF [9] = "Bad"
		PBIF [10] = "Bad"
		PBIF [11] = "Bad"
		PBIF [12] = "Bad"
		PBIF [0] = 1
	}

	Method(IVBS, 0)
	{
		PBST [0] = 0
		PBST [1] = 0xffffffff
		PBST [2] = 0xffffffff
		PBST [3] = 0xffffffff
	}
}


Device (BAT2)
{
	Name(_HID, EisaId("PNP0C0A"))
	Name(_UID, 2)

	Name(_PCL, Package(){ _SB })
	Name(PSTA, 0x1f)
	Name(PBIF, Package() {
		0x00,
		0x78,
		0x64,
		0x01,
		0x2b5c,	// Capacity?
		0x05,
		0x03,
		0x01,
		0x01,
		"???",	// Name
		"???",	// Number
		"???",	// Type
		"???"	// Vendor
	})
	Name(PBST, Package() { 0x01, 0x16, 0x64, 0x2b5c })

	/* Status */
	Method(_STA, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			If(\_SB.PCI0.LPCB.EC0.P63S) {
				Return(0x0f)
			} Else {
				Return(0x1f)
			}
		} Else {
			Return(0x0f)
		}
	}

	/* Battery Info */
	Method(_BIF, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			If(\_SB.PCI0.LPCB.EC0.P63S) {
				IVBI()
				IVBS()
			} Else {
				UPBI()
			}
		}

		Return(PBIF)
	}

	/* Battery Status */
	Method(_BST, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			UPBS()
		}

		Return(PBST)
	}

	/* Update Battery Info */
	Method(UPBI, 0)
	{
		PBIF [1] = 0x78
		PBIF [2] = 0x64
		PBIF [4] = 0x2b5c
		PBIF [9] = "Bat2"
		PBIF [10] = "002"
		PBIF [11] = "LION"
		PBIF [12] = "Panasonic"
	}

	Method(UPBS, 0)
	{
		Local0 = \_SB.PCI0.LPCB.EC0.QEVT
		If (~Local0) {
			GP38 = 0
			Sleep(0x64)
			Local0 = GP38
			If (~Local0) {
				Local0 = RDW (0x0d)
				If (Local0 != 0xeeee) {
					If (Local0 <= 0x64) {
						CBA2 = Local0
					}
				}
			}
		}

		Local0 = CBA2
		PBST [2] = Local0
		PBST [3] = DerefOf (PBIF [4])
		Local1 = 0

		If (PWRS) {
			If (Local0 < 0x64) {
				Local1 = 2
			}
		} Else {
			If (Local0 <= 0x5) {
				Local1 = 4
			} Else {
				Local1 = 1
			}
		}

		PBST [0] = Local1
		If (\_SB.PCI0.LPCB.EC0.P62S) {
			PBST [1] = 0x16
		} Else  {
			PBST [1] = 0x0b
		}
	}

	// Invalidate Battery Info
	Method(IVBI, 0)
	{
		PBIF [1] = 0xffffffff
		PBIF [2] = 0xffffffff
		PBIF [4] = 0xffffffff
		PBIF [9] = "Bad"
		PBIF [10] = "Bad"
		PBIF [11] = "Bad"
		PBIF [12] = "Bad"
		PBIF [0] = 1
	}

	Method(IVBS, 0)
	{
		PBST [0] = 0
		PBST [1] = 0xffffffff
		PBST [2] = 0xffffffff
		PBST [3] = 0xffffffff
	}
}

Method (RDW, 1)
{
	\_SB.PCI0.LPCB.EC0.SMAD = 0x16
	\_SB.PCI0.LPCB.EC0.SMCM = Arg0
	\_SB.PCI0.LPCB.EC0.SMPR = 0x09
	While (\_SB.PCI0.LPCB.EC0.SMPR != 0x00) {
		Stall (1)
	}

	Return (\_SB.PCI0.LPCB.EC0.SMW0)
}

Device (ADP1)
{
	Name (_HID, "ACPI0003")
	Method (_PSR, 0)
	{
		If (\_SB.PCI0.LPCB.EC0.ECON) {
			Local0 = \_SB.PCI0.LPCB.EC0.P60S
			If (Local0) {
				PWRS = 0
			} Else {
				PWRS = 1
			}
		}

		Stall (0x02)
		Return (PWRS)
	}

	Method (_PCL, 0)
	{
		Return (_SB)
	}
}
