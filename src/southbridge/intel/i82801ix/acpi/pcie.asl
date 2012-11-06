/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

/* Intel i82801G PCIe support */

// PCI Express Ports

Device (RP01)
{
	NAME(_ADR, 0x001c0000) // FIXME: Have a macro for PCI Devices -> ACPI notation?
	//#include "pcie_port.asl"
	Method(_PRT)
	{
		If (PICM) {
			Return (Package() {
				Package() { 0x0000ffff, 0, 0, 16 },
				Package() { 0x0000ffff, 1, 0, 17 },
				Package() { 0x0000ffff, 2, 0, 18 },
				Package() { 0x0000ffff, 3, 0, 19 }
			})
		} Else {
			Return (Package() {
				Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
				Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
				Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
				Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 }
			})

		}

	}
}

Device (RP02)
{
	NAME(_ADR, 0x001c0001) // FIXME: Have a macro for PCI Devices -> ACPI notation?
	//#include "pcie_port.asl"
	Method(_PRT)
	{
		If (PICM) {
			Return (Package() {
				Package() { 0x0000ffff, 0, 0, 17 },
				Package() { 0x0000ffff, 1, 0, 18 },
				Package() { 0x0000ffff, 2, 0, 19 },
				Package() { 0x0000ffff, 3, 0, 16 }
			})
		} Else {
			Return (Package() {
				Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKB, 0 },
				Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKC, 0 },
				Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKD, 0 },
				Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKA, 0 }
			})

		}

	}
}


Device (RP03)
{
	NAME(_ADR, 0x001c0002) // FIXME: Have a macro for PCI Devices -> ACPI notation?
	//#include "pcie_port.asl"
	Method(_PRT)
	{
		If (PICM) {
			Return (Package() {
				Package() { 0x0000ffff, 0, 0, 18 },
				Package() { 0x0000ffff, 1, 0, 19 },
				Package() { 0x0000ffff, 2, 0, 16 },
				Package() { 0x0000ffff, 3, 0, 17 }
			})
		} Else {
			Return (Package() {
				Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKC, 0 },
				Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
				Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKA, 0 },
				Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKB, 0 }
			})

		}

	}
}


Device (RP04)
{
	NAME(_ADR, 0x001c0003) // FIXME: Have a macro for PCI Devices -> ACPI notation?
	//#include "pcie_port.asl"
	Method(_PRT)
	{
		If (PICM) {
			Return (Package() {
				Package() { 0x0000ffff, 0, 0, 19 },
				Package() { 0x0000ffff, 1, 0, 16 },
				Package() { 0x0000ffff, 2, 0, 17 },
				Package() { 0x0000ffff, 3, 0, 18 }
			})
		} Else {
			Return (Package() {
				Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKD, 0 },
				Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKA, 0 },
				Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKB, 0 },
				Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKC, 0 }
			})

		}

	}
}


Device (RP05)
{
	NAME(_ADR, 0x001c0004) // FIXME: Have a macro for PCI Devices -> ACPI notation?
	//#include "pcie_port.asl"
	Method(_PRT)
	{
		If (PICM) {
			Return (Package() {
				Package() { 0x0000ffff, 0, 0, 16 },
				Package() { 0x0000ffff, 1, 0, 17 },
				Package() { 0x0000ffff, 2, 0, 18 },
				Package() { 0x0000ffff, 3, 0, 19 }
			})
		} Else {
			Return (Package() {
				Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
				Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
				Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
				Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 }
			})

		}

	}
}


Device (RP06)
{
	NAME(_ADR, 0x001c0005) // FIXME: Have a macro for PCI Devices -> ACPI notation?
	//#include "pcie_port.asl"
	Method(_PRT)
	{
		If (PICM) {
			Return (Package() {
				Package() { 0x0000ffff, 0, 0, 17 },
				Package() { 0x0000ffff, 1, 0, 18 },
				Package() { 0x0000ffff, 2, 0, 19 },
				Package() { 0x0000ffff, 3, 0, 16 }
			})
		} Else {
			Return (Package() {
				Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKB, 0 },
				Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKC, 0 },
				Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKD, 0 },
				Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKA, 0 }
			})

		}

	}
}


