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
 */

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
			If(\_SB.PCI0.LPCB.EC0.BAT) {
				Return(0x1f)
			} Else {
				Return(0x0f)
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
			Store(_STA(), Local0)
			And(Local0, 0x10, Local0)
			If(LNot(Local0)) {
				Return (PBIF)
			}

			Store(\_SB.PCI0.LPCB.EC0.BDC0, Index(PBIF, 1))
			Store(\_SB.PCI0.LPCB.EC0.BDV0, Index(PBIF, 4))

			Store(\_SB.PCI0.LPCB.EC0.BFC0, Local0)
			Store(Local0, Index(PBIF, 2))

			Divide(Local0, 10, , Local2)
			Store(Local2, Index(PBIF, 5))

			Divide(Local0, 20, , Local2)
			Store(Local2, Index(PBIF, 6))

			If(\_SB.PCI0.LPCB.EC0.BTYP) {
				Store(NIMH, Index(PBIF, 11))
			} Else {
				Store(LION, Index(PBIF, 11))
			}

			Return(PBIF)
		}

		/* Battery Status */
		Method(_BST, 0)
		{
			If(\_SB.PCI0.LPCB.EC0.BAT) {
				Store(\_SB.PCI0.LPCB.EC0.BPV0, Index(PBST, 3))

				Multiply(\_SB.PCI0.LPCB.EC0.BRC0, 100, Local3)
				Divide(Local3, \_SB.PCI0.LPCB.EC0.BFC0, Local3, Local0)
				Multiply(\_SB.PCI0.LPCB.EC0.BFC0, Local0, Local3)
				Divide(Local3, 0x64, Local3, Local0)
				Increment(Local0)
				Store(Local0, Index(PBST, 2))

				Store (\_SB.PCI0.LPCB.EC0.BRC0, Local3)
				Store (\_SB.PCI0.LPCB.EC0.BPR0, Local0)
				And (Not (Local0), 0xFFFF, Local0)
				Store (Local0, Index(PBST,1))

				// AC Power connected?
				If(\_SB.PCI0.LPCB.EC0.ADP) {
					If(\_SB.PCI0.LPCB.EC0.CHRG) {
						Store(2, Index(PBST, 0))
					} Else {
						Store(0, Index(PBST, 0))
					}
				} Else {
					If(LLess(Local3, 25)) {
						Store(5, Index(PBST, 0))
					} Else {
						Store(1, Index(PBST, 0))
					}
				}
			}
			Return(PBST)
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
			If(\_SB.PCI0.LPCB.EC0.BAT2) {
				Return(0x1f)
			} Else {
				Return(0x0f)
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
			Store(_STA(), Local0)
			And(Local0, 0x10, Local0)
			If(LNot(Local0)) {
				Return (PBIF)
			}

			Store(\_SB.PCI0.LPCB.EC0.BDC2, Index(PBIF, 1))
			Store(\_SB.PCI0.LPCB.EC0.BDV2, Index(PBIF, 4))

			Store(\_SB.PCI0.LPCB.EC0.BFC2, Local0)
			Store(Local0, Index(PBIF, 2))

			Divide(Local0, 10, , Local2)
			Store(Local2, Index(PBIF, 5))

			Divide(Local0, 20, , Local2)
			Store(Local2, Index(PBIF, 6))

			If(\_SB.PCI0.LPCB.EC0.BTY2) {
				Store(NIMH, Index(PBIF, 11))
			} Else {
				Store(LION, Index(PBIF, 11))
			}

			Return(PBIF)
		}

		/* Battery Status */
		Method(_BST, 0)
		{
			If(\_SB.PCI0.LPCB.EC0.BAT2) {
				Store(\_SB.PCI0.LPCB.EC0.BPV2, Index(PBST, 3))

				Multiply(\_SB.PCI0.LPCB.EC0.BRC2, 100, Local3)
				Divide(Local3, \_SB.PCI0.LPCB.EC0.BFC2, Local3, Local0)
				Multiply(\_SB.PCI0.LPCB.EC0.BFC2, Local0, Local3)
				Divide(Local3, 0x64, Local3, Local0)
				Increment(Local0)
				Store(Local0, Index(PBST, 2))

				Store (\_SB.PCI0.LPCB.EC0.BRC2, Local3)
				Store (\_SB.PCI0.LPCB.EC0.BPR2, Local0)
				And (Not (Local0), 0xFFFF, Local0)
				Store (Local0, Index(PBST,1))

				// AC Power connected?
				If(\_SB.PCI0.LPCB.EC0.ADP) {
					If(\_SB.PCI0.LPCB.EC0.CRG2) {
						Store(2, Index(PBST, 0))
					} Else {
						Store(0, Index(PBST, 0))
					}
				} Else {
					If(LLess(Local3, 25)) {
						Store(5, Index(PBST, 0))
					} Else {
						Store(1, Index(PBST, 0))
					}
				}
			}
			Return(PBST)
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
			If(ACFG) {
				Store(ACST, Local0)
			} Else {
				Store(\_SB.PCI0.LPCB.EC0.ADP, Local0)
				Store(Local0, ACST)
				Store(1, ACFG)
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
