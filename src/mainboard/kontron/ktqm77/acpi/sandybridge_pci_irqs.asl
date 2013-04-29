/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* This is board specific information: IRQ routing for Ivybridge */

// PCI Interrupt Routing
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			// LPC devices			0:1f.x
							   //  D31IP_TTIP  THRT   INTC -> PIRQC
			Package() { 0x001fffff, 2, 0, 18 },//  D31IP_SMIP  SMBUS  INTC -> PIRQC
			Package() { 0x001fffff, 1, 0, 19 },//  D31IP_SIP   SATA   INTB -> PIRQD (MSI)
			// EHCI	#1			0:1d.0
			Package() { 0x001dffff, 0, 0, 23 },//  D29IP_E1P   EHCI1  INTA -> PIRQH
			// PCIe Root Ports		0:1c.x
							   //  D28IP_P8IP  Slot?  INTD -> PIRQD
			Package() { 0x001cffff, 3, 0, 19 },//  D28IP_P4IP  ETH2   INTD -> PIRQD (MSI)
							   //  D28IP_P7IP  PCIEx1 INTC -> PIRQC
			Package() { 0x001cffff, 2, 0, 18 },//  D28IP_P3IP  ETH1   INTC -> PIRQC (MSI)
							   //  D28IP_P6IP  1394   INTB -> PIRQB (MSI)
			Package() { 0x001cffff, 1, 0, 17 },//  D28IP_P2IP  Slot?  INTB -> PIRQB
							   //  D28IP_P5IP  GbEPHY INTA -> PIRQA
			Package() { 0x001cffff, 0, 0, 16 },//  D28IP_P1IP  Slot?  INTA -> PIRQA
			// High Definition Audio	0:1b.0
			Package() { 0x001bffff, 0, 0, 22 },//  D27IP_ZIP   HDA    INTA -> PIRQG (MSI)
			// EHCI	#2			0:1a.0
			Package() { 0x001affff, 0, 0, 16 },//  D26IP_E2P   EHCI2  INTA -> PIRQA
			// ETH0				0:19.0
			Package() { 0x0019ffff, 0, 0, 20 },//  D25IP_LIP   ETH0   INTA -> PIRQE (MSI)
			// xHCI				0:14.0
			Package() { 0x0014ffff, 0, 0, 16 },//  D20IP_XHCIIP xHCI  INTA -> PIRQA (MSI)
			// Onboard graphics (IGD)	0:2.0
			Package() { 0x0002ffff, 0, 0, 16 },//              GFX    INTA -> PIRQA (MSI)
			// PCIe PEG x16			0:1.0
			Package() { 0x0001ffff, 3, 0, 19 },//              PEGx16 INTD -> PIRQD
			Package() { 0x0001ffff, 2, 0, 18 },//              PEGx16 INTC -> PIRQC
			Package() { 0x0001ffff, 1, 0, 17 },//              PEGx16 INTB -> PIRQB
			Package() { 0x0001ffff, 0, 0, 16 },//              PEGx16 INTA -> PIRQA
		})
	} Else {
		Return (Package() {
			// LPC devices			0:1f.x
			Package() { 0x001fffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001fffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			// EHCI	#1			0:1d.0
			Package() { 0x001dffff, 0, \_SB.PCI0.LPCB.LNKH, 0 },
			// PCIe Root Ports		0:1c.x
			Package() { 0x001cffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001cffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001cffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001cffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			// High Definition Audio	0:1b.0
			Package() { 0x001bffff, 0, \_SB.PCI0.LPCB.LNKG, 0 },
			// EHCI	#2			0:1a.0
			Package() { 0x001affff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			// ETH0				0:19.0
			Package() { 0x0019ffff, 0, \_SB.PCI0.LPCB.LNKE, 0 },
			// xHCI				0:14.0
			Package() { 0x0014ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			// Onboard graphics (IGD)	0:2.0
			Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			// PCIe PEG x16			0:1.0
			Package() { 0x0001ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
		})
	}
}

