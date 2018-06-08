/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 - 2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors.  All Rights Reserved.
 * Copyright (C) 2014 - 2017 Intel Corporation.
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
 */

/* Intel 6/7 Series PCH PCIe support */

// PCI Express Ports

Method (IRQM, 1, Serialized) {

	/* Interrupt Map INTA->INTA, INTB->INTB, INTC->INTC, INTD->INTD */
	Name (IQAA, Package() {
		Package() { 0x0000ffff, 0, 0, 16 },
		Package() { 0x0000ffff, 1, 0, 17 },
		Package() { 0x0000ffff, 2, 0, 18 },
		Package() { 0x0000ffff, 3, 0, 19 } })
	Name (IQAP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 } })

	/* Interrupt Map INTA->INTB, INTB->INTC, INTC->INTD, INTD->INTA */
	Name (IQBA, Package() {
		Package() { 0x0000ffff, 0, 0, 17 },
		Package() { 0x0000ffff, 1, 0, 18 },
		Package() { 0x0000ffff, 2, 0, 19 },
		Package() { 0x0000ffff, 3, 0, 16 } })
	Name (IQBP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKB, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKC, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKD, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKA, 0 } })

	/* Interrupt Map INTA->INTC, INTB->INTD, INTC->INTA, INTD->INTB */
	Name (IQCA, Package() {
		Package() { 0x0000ffff, 0, 0, 18 },
		Package() { 0x0000ffff, 1, 0, 19 },
		Package() { 0x0000ffff, 2, 0, 16 },
		Package() { 0x0000ffff, 3, 0, 17 } })
	Name (IQCP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKC, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKA, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKB, 0 } })

	/* Interrupt Map INTA->INTD, INTB->INTA, INTC->INTB, INTD->INTC */
	Name (IQDA, Package() {
		Package() { 0x0000ffff, 0, 0, 19 },
		Package() { 0x0000ffff, 1, 0, 16 },
		Package() { 0x0000ffff, 2, 0, 17 },
		Package() { 0x0000ffff, 3, 0, 18 } })
	Name (IQDP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKD, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKA, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKB, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKC, 0 } })

	/* Interrupt Map INTA->INTE, INTB->INTF, INTC->INTG, INTD->INTH */
	Name (IQEA, Package() {
		Package() { 0x0000ffff, 0, 0, 20 },
		Package() { 0x0000ffff, 1, 0, 21 },
		Package() { 0x0000ffff, 2, 0, 22 },
		Package() { 0x0000ffff, 3, 0, 23 } })
	Name (IQEP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKE, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKF, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKG, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKH, 0 } })

	/* Interrupt Map INTA->INTF, INTB->INTG, INTC->INTH, INTD->INTE */
	Name (IQFA, Package() {
		Package() { 0x0000ffff, 0, 0, 21 },
		Package() { 0x0000ffff, 1, 0, 22 },
		Package() { 0x0000ffff, 2, 0, 23 },
		Package() { 0x0000ffff, 3, 0, 20 } })
	Name (IQFP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKF, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKG, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKH, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKE, 0 } })

	/* Interrupt Map INTA->INTG, INTB->INTH, INTC->INTE, INTD->INTF */
	Name (IQGA, Package() {
		Package() { 0x0000ffff, 0, 0, 22 },
		Package() { 0x0000ffff, 1, 0, 23 },
		Package() { 0x0000ffff, 2, 0, 20 },
		Package() { 0x0000ffff, 3, 0, 21 } })
	Name (IQGP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKG, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKH, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKE, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKF, 0 } })

	/* Interrupt Map INTA->INTH, INTB->INTE, INTC->INTF, INTD->INTG */
	Name (IQHA, Package() {
		Package() { 0x0000ffff, 0, 0, 23 },
		Package() { 0x0000ffff, 1, 0, 20 },
		Package() { 0x0000ffff, 2, 0, 21 },
		Package() { 0x0000ffff, 3, 0, 22 } })
	Name (IQHP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKH, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKE, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKF, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKG, 0 } })

	Switch (ToInteger (Arg0)) {
		/* PCIe Root Port 1 */
		Case (Package() { 1 }) {
			If (PICM) {
				Return (IQAA)
			} Else {
				Return (IQAP)
			}
		}

		/* PCIe Root Port 2 */
		Case (Package() { 2 }) {
			If (PICM) {
				Return (IQBA)
			} Else {
				Return (IQBP)
			}
		}

		/* PCIe Root Port 3 */
		Case (Package() { 3 }) {
			If (PICM) {
				Return (IQCA)
			} Else {
				Return (IQCP)
			}
		}

		/* PCIe Root Port 4 */
		Case (Package() { 4 }) {
			If (PICM) {
				Return (IQDA)
			} Else {
				Return (IQDP)
			}
		}

		/* PCIe Root Port 5 */
		Case (Package() { 5 }) {
			If (PICM) {
				Return (IQEA)
			} Else {
				Return (IQEP)
			}
		}

		/* PCIe Root Port 6 */
		Case (Package() { 6 }) {
			If (PICM) {
				Return (IQFA)
			} Else {
				Return (IQFP)
			}
		}

		/* PCIe Root Port 7 */
		Case (Package() { 7 }) {
			If (PICM) {
				Return (IQGA)
			} Else {
				Return (IQGP)
			}
		}

		/* PCIe Root Port 8 */
		Case (Package() { 8 }) {
			If (PICM) {
				Return (IQHA)
			} Else {
				Return (IQHP)
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
	Name (_ADR, 0x00090000)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP02)
{
	Name (_ADR, 0x000A0000)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP03)
{
	Name (_ADR, 0x000B0000)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP04)
{
	Name (_ADR, 0x000C0000)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP05)
{
	Name (_ADR, 0x000E0000)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP06)
{
	Name (_ADR, 0x000F0000)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP07)
{
	Name (_ADR, 0x00100000)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP08)
{
	Name (_ADR, 0x00110000)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}
