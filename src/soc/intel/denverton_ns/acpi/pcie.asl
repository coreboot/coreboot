/* SPDX-License-Identifier: GPL-2.0-only */

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

	/* Interrupt Map INTA->INTC, INTB->INTB, INTC->INTC, INTD->INTD */
	Name (IQIA, Package() {
		Package() { 0x0000ffff, 0, 0, 18 },
		Package() { 0x0000ffff, 1, 0, 17 },
		Package() { 0x0000ffff, 2, 0, 18 },
		Package() { 0x0000ffff, 3, 0, 19 } })
	Name (IQIP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKC, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 } })

	/* Interrupt Map INTA->INTA, INTB->INTB, INTC->INTC, INTD->INTD */
	Name (IQJA, Package() {
		Package() { 0x0000ffff, 0, 0, 23 },
		Package() { 0x0000ffff, 1, 0, 20 },
		Package() { 0x0000ffff, 2, 0, 21 },
		Package() { 0x0000ffff, 3, 0, 22 } })
	Name (IQJP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 } })

	/* Interrupt Map INTA->INTB, INTB->INTB, INTC->INTC, INTD->INTD */
	Name (IQKA, Package() {
		Package() { 0x0000ffff, 0, 0, 17 },
		Package() { 0x0000ffff, 1, 0, 17 },
		Package() { 0x0000ffff, 2, 0, 18 },
		Package() { 0x0000ffff, 3, 0, 19 } })
	Name (IQKP, Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKB, 0 },
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 } })

	Switch (ToInteger (Arg0)) {
		/* Virtual Root Port 2 - QAT */
		Case (Package() { 6 }) {
			If (PICM) {
				Return (IQIA)
			} Else {
				Return (IQIP)
			}
		}

		/* PCIe Root Port 1 */
		Case (Package() { 9 }) {
			If (PICM) {
				Return (IQAA)
			} Else {
				Return (IQAP)
			}
		}

		/* PCIe Root Port 2 */
		Case (Package() { 10 }) {
			If (PICM) {
				Return (IQBA)
			} Else {
				Return (IQBP)
			}
		}

		/* PCIe Root Port 3 */
		Case (Package() { 11 }) {
			If (PICM) {
				Return (IQCA)
			} Else {
				Return (IQCP)
			}
		}

		/* PCIe Root Port 4 */
		Case (Package() { 12 }) {
			If (PICM) {
				Return (IQDA)
			} Else {
				Return (IQDP)
			}
		}

		/* PCIe Root Port 5 */
		Case (Package() { 14 }) {
			If (PICM) {
				Return (IQEA)
			} Else {
				Return (IQEP)
			}
		}

		/* PCIe Root Port 6 */
		Case (Package() { 15 }) {
			If (PICM) {
				Return (IQFA)
			} Else {
				Return (IQFP)
			}
		}

		/* PCIe Root Port 7 */
		Case (Package() { 16 }) {
			If (PICM) {
				Return (IQGA)
			} Else {
				Return (IQGP)
			}
		}

		/* PCIe Root Port 8 */
		Case (Package() { 17 }) {
			If (PICM) {
				Return (IQHA)
			} Else {
				Return (IQHP)
			}
		}

		/* Virtual Root Port 0 - LAN 0 */
		Case (Package() { 22 }) {
			If (PICM) {
				Return (IQJA)
			} Else {
				Return (IQJP)
			}
		}

		/* Virtual Root Port 1 - LAN 1 */
		Case (Package() { 23 }) {
			If (PICM) {
				Return (IQKA)
			} Else {
				Return (IQKP)
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
