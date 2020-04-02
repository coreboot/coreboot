/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* This is board specific information: IRQ routing */

// PCI Interrupt Routing
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			// [GREG]: Global Registers
			Package() { 0x0004ffff, 0, 0, 16 },

			// [RCEC]: Root Complex Event Collector
			Package() { 0x0005ffff, 0, 0, 23 },

			// [VRP2]: Virtual root port 2
			Package() { 0x0006ffff, 2, 0, 18 },

			// [PEX0]: PCI Express Port 0
			Package() { 0x0009ffff, 0, 0, 16 },

			// [PEX1]: PCI Express Port 1
			Package() { 0x000affff, 1, 0, 17 },

			// [PEX2]: PCI Express Port 2
			Package() { 0x000bffff, 2, 0, 18 },

			// [PEX3]: PCI Express Port 3
			Package() { 0x000cffff, 3, 0, 19 },

			// [PEX4]: PCI Express Port 4
			Package() { 0x000effff, 0, 0, 20 },

			// [PEX5]: PCI Express Port 5
			Package() { 0x000fffff, 1, 0, 21 },

			// [PEX6]: PCI Express Port 6
			Package() { 0x0010ffff, 2, 0, 22 },

			// [PEX7]: PCI Express Port 7
			Package() { 0x0011ffff, 3, 0, 23 },

			// [SMB1]: SMBus controller
			Package() { 0x0012ffff, 0, 0, 16 },

			// [SAT0]: SATA controller 0
			Package() { 0x0013ffff, 0, 0, 20 },

			// [SAT1]: SATA controller 1
			Package() { 0x0014ffff, 0, 0, 21 },

			// [XHC0]: XHCI USB controller
			Package() { 0x0015ffff, 0, 0, 19 },

			// [VRP0]: Virtual root port 0
			Package() { 0x0016ffff, 0, 0, 16 },

			// [VRP1]: Virtual root port 1
			Package() { 0x0017ffff, 1, 0, 17 },

			// [HECI]: ME HECI
			Package() { 0x0018ffff, 0, 0, 16 },

			// [HEC2]: ME HECI2
			Package() { 0x0018ffff, 1, 0, 17 },

			// [MEKT]: MEKT on PCH
			Package() { 0x0018ffff, 2, 0, 18 },

			// [HEC3]: ME HECI3
			Package() { 0x0018ffff, 3, 0, 19 },

			// [UAR0]: UART 0
			Package() { 0x001affff, 0, 0, 16 },

			// [UAR1]: UART 1
			Package() { 0x001affff, 1, 0, 17 },

			// [UAR2]: UART 2
			Package() { 0x001affff, 2, 0, 18 },

			// [EMMC]: eMMC
			Package() { 0x001cffff, 0, 0, 16 },

			// [P2SB]: Primary to sideband bridge
			// [SMB0]: SMBus controller
			// [NPK0]: Northpeak DFX
			Package() { 0x001fffff, 0, 0, 23 },
		})
	} Else {
		Return (Package() {
			// [GREG]: Global Registers		0:4.0
			Package() { 0x0004ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },

			// [RCEC]: Root Complex Event Collector		0:5.0
			Package() { 0x0005ffff, 0, \_SB.PCI0.LPCB.LNKH, 0 },

			// [VRP2]: Virtual root port 2		0:6.0
			Package() { 0x0006ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },

			// [PEX0]: PCI Express Port 0		0:9.0
			Package() { 0x0009ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },

			// [PEX1]: PCI Express Port 1		0:a.0
			Package() { 0x000affff, 1, \_SB.PCI0.LPCB.LNKB, 0 },

			// [PEX2]: PCI Express Port 2		0:b.0
			Package() { 0x000bffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },

			// [PEX3]: PCI Express Port 3		0:c.0
			Package() { 0x000cffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },

			// [PEX4]: PCI Express Port 4		0:e.0
			Package() { 0x000effff, 0, \_SB.PCI0.LPCB.LNKE, 0 },

			// [PEX5]: PCI Express Port 5		0:f.0
			Package() { 0x000fffff, 1, \_SB.PCI0.LPCB.LNKF, 0 },

			// [PEX6]: PCI Express Port 6		0:10.0
			Package() { 0x0010ffff, 2, \_SB.PCI0.LPCB.LNKG, 0 },

			// [PEX7]: PCI Express Port 7		0:11.0
			Package() { 0x0011ffff, 3, \_SB.PCI0.LPCB.LNKH, 0 },

			// [SMB1]: SMBus controller		0:12.0
			Package() { 0x0012ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },

			// [SAT0]: SATA controller 0		0:13.0
			Package() { 0x0013ffff, 0, \_SB.PCI0.LPCB.LNKE, 0 },

			// [SAT1]: SATA controller 1		0:14.0
			Package() { 0x0014ffff, 0, \_SB.PCI0.LPCB.LNKF, 0 },

			// [XHC0]: XHCI USB controller		0:15.0
			Package() { 0x0015ffff, 0, \_SB.PCI0.LPCB.LNKD, 0 },

			// [VRP0]: Virtual root port 0		0:16.0
			Package() { 0x0016ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },

			// [VRP1]: Virtual root port 1		0:17.0
			Package() { 0x0017ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },

			// [HECI]: ME HECI		0:18.0
			Package() { 0x0018ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },

			// [HEC2]: ME HECI2		0:18.1
			Package() { 0x0018ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },

			// [MEKT]: MEKT on PCH		0:18.2
			Package() { 0x0018ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },

			// [HEC3]: ME HECI3		0:18.3
			Package() { 0x0018ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },

			// [UAR0]: UART 0		0:1a.0
			Package() { 0x001affff, 0, \_SB.PCI0.LPCB.LNKA, 0 },

			// [UAR1]: UART 1		0:1a.1
			Package() { 0x001affff, 1, \_SB.PCI0.LPCB.LNKB, 0 },

			// [UAR2]: UART 2		0:1a.2
			Package() { 0x001affff, 2, \_SB.PCI0.LPCB.LNKC, 0 },

			// [EMMC]: eMMC		0:1c.0
			Package() { 0x001cffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },

			// [P2SB]: Primary to sideband bridge
			// [SMB0]: SMBus controller
			// [NPK0]: Northpeak DFX
			Package() { 0x001ffffF, 0, \_SB.PCI0.LPCB.LNKH, 0 },
		})
	}
}
