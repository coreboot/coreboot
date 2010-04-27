/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

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
		Store (0x78, Index(PBIF, 1))
		Store (0x64, Index(PBIF, 2))
		Store (0x2b5c, Index(PBIF, 4))
		Store ("Bat1", Index(PBIF, 9))
		Store ("001", Index(PBIF, 10))
		Store ("LION", Index(PBIF, 11))
		Store ("Panasonic", Index(PBIF, 12))
	}

	Method(UPBS, 0)
	{
		Store(\_SB.PCI0.LPCB.EC0.QEVT, Local0)
		If (Not(Local0)) {
			Store(0, GP38)
			Sleep(0x64)
			Store(GP38, Local0)
			If (Not(Local0)) {
				Store (RDW(0x0d), Local0)
				If (LNotEqual(Local0, 0xeeee)) {
					If (LLessEqual(Local0, 0x64)) {
						Store(Local0, CBA1)
					}
				}
			}
		}

		Store (CBA1, Local0)
		Store (Local0, Index(PBST, 2))
		Store (DerefOf(Index(PBIF, 4)), Index(PBST, 3))
		Store (0, Local1)

		If (PWRS) {
			If (LLess(Local0, 0x64)) {
				Store (2, Local1)
			}
		} Else {
			If (LLessEqual(Local0, 0x5)) {
				Store (4, Local1)
			} Else {
				Store (1, Local1)
			}
		}

		Store (Local1, Index(PBST, 0))
		If (\_SB.PCI0.LPCB.EC0.P63S) {
			Store (0x16, Index(PBST, 1))
		} Else  {
			Store (0x0b, Index(PBST, 1))
		}
	}

	// Invalidate Battery Info
	Method(IVBI, 0)
	{
		Store (0xffffffff, Index(PBIF, 1))
		Store (0xffffffff, Index(PBIF, 2))
		Store (0xffffffff, Index(PBIF, 4))
		Store ("Bad", Index(PBIF, 9))
		Store ("Bad", Index(PBIF, 10))
		Store ("Bad", Index(PBIF, 11))
		Store ("Bad", Index(PBIF, 12))
		Store (1, PBIF)
	}

	Method(IVBS, 0)
	{
		Store (0x0, Index(PBST, 0))
		Store (0xffffffff, Index(PBST, 1))
		Store (0xffffffff, Index(PBST, 2))
		Store (0xffffffff, Index(PBST, 3))
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
		Store (0x78, Index(PBIF, 1))
		Store (0x64, Index(PBIF, 2))
		Store (0x2b5c, Index(PBIF, 4))
		Store ("Bat2", Index(PBIF, 9))
		Store ("002", Index(PBIF, 10))
		Store ("LION", Index(PBIF, 11))
		Store ("Panasonic", Index(PBIF, 12))
	}

	Method(UPBS, 0)
	{
		Store(\_SB.PCI0.LPCB.EC0.QEVT, Local0)
		If (Not(Local0)) {
			Store(0, GP38)
			Sleep(0x64)
			Store(GP38, Local0)
			If (Not(Local0)) {
				Store (RDW(0x0d), Local0)
				If (LNotEqual(Local0, 0xeeee)) {
					If (LLessEqual(Local0, 0x64)) {
						Store(Local0, CBA2)
					}
				}
			}
		}

		Store (CBA2, Local0)
		Store (Local0, Index(PBST, 2))
		Store (DerefOf(Index(PBIF, 4)), Index(PBST, 3))
		Store (0, Local1)

		If (PWRS) {
			If (LLess(Local0, 0x64)) {
				Store (2, Local1)
			}
		} Else {
			If (LLessEqual(Local0, 0x5)) {
				Store (4, Local1)
			} Else {
				Store (1, Local1)
			}
		}

		Store (Local1, Index(PBST, 0))
		If (\_SB.PCI0.LPCB.EC0.P62S) {
			Store (0x16, Index(PBST, 1))
		} Else  {
			Store (0x0b, Index(PBST, 1))
		}
	}

	// Invalidate Battery Info
	Method(IVBI, 0)
	{
		Store (0xffffffff, Index(PBIF, 1))
		Store (0xffffffff, Index(PBIF, 2))
		Store (0xffffffff, Index(PBIF, 4))
		Store ("Bad", Index(PBIF, 9))
		Store ("Bad", Index(PBIF, 10))
		Store ("Bad", Index(PBIF, 11))
		Store ("Bad", Index(PBIF, 12))
		Store (1, PBIF)
	}

	Method(IVBS, 0)
	{
		Store (0x0, Index(PBST, 0))
		Store (0xffffffff, Index(PBST, 1))
		Store (0xffffffff, Index(PBST, 2))
		Store (0xffffffff, Index(PBST, 3))
	}
}

Method (RDW, 1)
{
	Store (0x16, \_SB.PCI0.LPCB.EC0.SMAD)
	Store (Arg0, \_SB.PCI0.LPCB.EC0.SMCM)
	Store (0x09, \_SB.PCI0.LPCB.EC0.SMPR)
	While (LNotEqual(\_SB.PCI0.LPCB.EC0.SMPR, 0x00)) {
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
			Store (\_SB.PCI0.LPCB.EC0.P60S, Local0)
			If (Local0) {
				Store (0, PWRS)
			} Else {
				Store (1, PWRS)
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

