/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/* Intel PCH PCIe support */

Method (IRQM, 1, Serialized) {

	/* Interrupt Map INTA->INTA, INTB->INTB, INTC->INTC, INTD->INTD */
	Name (IQAA, Package() {
		Package() { 0x0000ffff, 0, 0, 16 },
		Package() { 0x0000ffff, 1, 0, 17 },
		Package() { 0x0000ffff, 2, 0, 18 },
		Package() { 0x0000ffff, 3, 0, 19 } })
	Name (IQAP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LNKA, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LNKB, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LNKC, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LNKD, 0 } })

	/* Interrupt Map INTA->INTB, INTB->INTC, INTC->INTD, INTD->INTA */
	Name (IQBA, Package() {
		Package() { 0x0000ffff, 0, 0, 17 },
		Package() { 0x0000ffff, 1, 0, 18 },
		Package() { 0x0000ffff, 2, 0, 19 },
		Package() { 0x0000ffff, 3, 0, 16 } })
	Name (IQBP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LNKB, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LNKC, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LNKD, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LNKA, 0 } })

	/* Interrupt Map INTA->INTC, INTB->INTD, INTC->INTA, INTD->INTB */
	Name (IQCA, Package() {
		Package() { 0x0000ffff, 0, 0, 18 },
		Package() { 0x0000ffff, 1, 0, 19 },
		Package() { 0x0000ffff, 2, 0, 16 },
		Package() { 0x0000ffff, 3, 0, 17 } })
	Name (IQCP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LNKC, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LNKD, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LNKA, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LNKB, 0 } })

	/* Interrupt Map INTA->INTD, INTB->INTA, INTC->INTB, INTD->INTC */
	Name (IQDA, Package() {
		Package() { 0x0000ffff, 0, 0, 19 },
		Package() { 0x0000ffff, 1, 0, 16 },
		Package() { 0x0000ffff, 2, 0, 17 },
		Package() { 0x0000ffff, 3, 0, 18 } })
	Name (IQDP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LNKD, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LNKA, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LNKB, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LNKC, 0 } })

	Switch (ToInteger (Arg0)) {
		/* PCIe Root Port 1 and 5 */
		Case (Package() { 1, 5 }) {
			If (PICM) {
				Return (IQAA)
			} Else {
				Return (IQAP)
			}
		}

		/* PCIe Root Port 2 and 6 */
		Case (Package() { 2, 6 }) {
			If (PICM) {
				Return (IQBA)
			} Else {
				Return (IQBP)
			}
		}

		/* PCIe Root Port 3 and 7 */
		Case (Package() { 3, 7 }) {
			If (PICM) {
				Return (IQCA)
			} Else {
				Return (IQCP)
			}
		}

		/* PCIe Root Port 4 and 8 */
		Case (Package() { 4, 8 }) {
			If (PICM) {
				Return (IQDA)
			} Else {
				Return (IQDP)
			}
		}

		Default {
			If (PICM) {
				Return (IQDA)
			} Else {
				Return (IQDP)
			}
		}
	}
}

Device (RP01)
{
	Name (_ADR, 0x001c0000)

	Method (_PRT)
	{
		Return (IRQM (RPA1))
	}
}

Device (RP02)
{
	Name (_ADR, 0x001c0001)

	Method (_PRT)
	{
		Return (IRQM (RPA2))
	}
}

Device (RP03)
{
	Name (_ADR, 0x001c0002)

	Method (_PRT)
	{
		Return (IRQM (RPA3))
	}
}

Device (RP04)
{
	Name (_ADR, 0x001c0003)

	Method (_PRT)
	{
		Return (IRQM (RPA4))
	}
}

Device (RP05)
{
	Name (_ADR, 0x001c0004)

	Method (_PRT)
	{
		Return (IRQM (RPA5))
	}
}

Device (RP06)
{
	Name (_ADR, 0x001c0005)

	Method (_PRT)
	{
		Return (IRQM (RPA6))
	}
}

Device (RP07)
{
	Name (_ADR, 0x001c0006)

	Method (_PRT)
	{
		Return (IRQM (RPA7))
	}
}

Device (RP08)
{
	Name (_ADR, 0x001c0007)

	Method (_PRT)
	{
		Return (IRQM (RPA8))
	}
}
Device (RP09)
{
        Name (_ADR, 0x001D0000)

        Method (_PRT)
        {
		Return (IRQM (RPA9))
        }
}

Device (RP10)
{
        Name (_ADR, 0x001D0001)

        Method (_PRT)
        {
		Return (IRQM (RPAA))
        }
}

Device (RP11)
{
        Name (_ADR, 0x001D0002)

        Method (_PRT)
        {
		Return (IRQM (RPAB))
        }
}

Device (RP12)
{
        Name (_ADR, 0x001D0003)

        Method (_PRT)
        {
		Return (IRQM (RPAC))
        }
}

