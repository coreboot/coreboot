/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel PCH PCIe support */

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

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP02)
{
	Name (_ADR, 0x001c0001)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP03)
{
	Name (_ADR, 0x001c0002)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP04)
{
	Name (_ADR, 0x001c0003)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP05)
{
	Name (_ADR, 0x001c0004)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP06)
{
	Name (_ADR, 0x001c0005)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP07)
{
	Name (_ADR, 0x001c0006)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP08)
{
	Name (_ADR, 0x001c0007)

	#include "pcie_port.asl"

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}
