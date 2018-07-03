/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Raptor Engineering
 * Copyright (C) 2005 - 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2004 Nick Barker <Nick.Barker9@btinternet.com>
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * WARNING: Sleep/Wake is a work in progress and is still somewhat flaky!
 * Everything else does to the best of my knowledge... (T.P. 01/26/2015)
 */

/*
 * ISA portions taken from QEMU acpi-dsdt.dsl.
 */

/*
 * PCI link routing templates taken from ck804.asl and modified for this board
 */

DefinitionBlock (
		"DSDT.AML",	/* Output filename */
		"DSDT",		/* Signature */
		0x03,		/* DSDT Revision, needs to be 2 or higher for 64bit */
		"ASUS  ",	/* OEMID */
		"COREBOOT",	/* TABLE ID */
		0x00000001	/* OEM Revision */
		)
{
	#include <northbridge/amd/amdfam10/amdfam10_util.asl>
	#include <southbridge/amd/sr5650/acpi/sr5650.asl>

	/* Some global data */
	Name(OSVR, 3)	/* Assume nothing. WinXp = 1, Vista = 2, Linux = 3, WinCE = 4 */
	Name(OSV, Ones)	/* Assume nothing */
	Name(PICM, One)	/* Assume APIC */

	/* HPET enable */
	Name (HPTE, 0x1)

	#include <southbridge/amd/common/acpi/sleepstates.asl>

	/* The _PIC method is called by the OS to choose between interrupt
		* routing via the i8259 interrupt controller or the APIC.
		*
		* _PIC is called with a parameter of 0 for i8259 configuration and
		* with a parameter of 1 for Local Apic/IOAPIC configuration.
		*/
	Method (_PIC, 1, Serialized) {
		If (Arg0)
		{
			\_SB.CIRQ()
		}
		Store (Arg0, PICM)
	}

	/* _PR CPU0 is dynamically supplied by SSDT */
	/* CPU objects and _PSS entries are dynamically supplied by SSDT */

	Scope(\_GPE) {	/* Start Scope GPE */
		/*  General event 3  */
		Method(_L03) {
			/* Level-Triggered GPE */
			Notify(\_SB.PWRB, 0x02)			/* NOTIFY_DEVICE_WAKE */
		}

		/*  General event 4  */
		Method(_L04) {
			/* Level-Triggered GPE */
			Notify (\_SB.PCI0.PBR0, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PWRB, 0x02)		/* NOTIFY_DEVICE_WAKE */
		}

		/*  Keyboard controller PME#  */
		Method(_L08) {
			/* Level-Triggered GPE */
			Notify(\_SB.PCI0.LPC.PS2K, 0x02)	/* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.LPC.PS2M, 0x02)	/* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02)			/* NOTIFY_DEVICE_WAKE */
		}

		/*  USB controller PME#  */
		Method(_L0B) {
			/* Level-Triggered GPE */
			Notify (\_SB.PCI0.USB0, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.USB1, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.USB2, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.USB3, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.USB4, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.USB5, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.USB6, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PWRB, 0x02)		/* NOTIFY_DEVICE_WAKE */
		}

		/*  GPIO0 or GEvent8 event  */
		Method(_L18) {
			/* Level-Triggered GPE */
			Notify (\_SB.PCI0.PCE1, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.NICA, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.NICB, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.PCE4, 0x02)		/* NOTIFY_DEVICE_WAKE */
		}

	}	/* End Scope GPE */

	/* Root of the bus hierarchy */
	Scope (\_SB)
	{
		/* Top southbridge PCI device (SR5670 + SP5100) */
		Device (PCI0)
		{
			/* BUS0 root bus */

			Name (_HID, EisaId ("PNP0A08"))		/* PCI-e root bus (SR5670) */
			Name (_CID, EisaId ("PNP0A03"))		/* PCI root bus (SP5100) */
			Name (_ADR, 0x00180001)
			Name (_UID, 0x00)

			Name (HCIN, 0x00)  // HC1

			Method (_BBN, 0, NotSerialized)
			{
				Return (GBUS (GHCN(HCIN), GHCL(HCIN)))
			}

			/* Operating System Capabilities Method */
			Method(_OSC,4)
			{
				/* Let OS control everything */
				Return (Arg3)
			}

			External (BUSN)
			External (MMIO)
			External (PCIO)
			External (SBLK)
			External (TOM1)
			External (HCLK)
			External (SBDN)
			External (HCDN)
			External (CBST)

			/* PCI Routing Tables */
			Name (PR00, Package () {
				/* PIC */
				/* Top southbridge device (SR5670) */
				/* HT Link */
				Package (0x04) { 0x0000FFFF, 0x00, LNKA, 0x00 },

				/* PCI-E Slot 1 (Bridge) */
				Package (0x04) { 0x0002FFFF, 0x00, LNKE, 0x00 },

				/* NIC A (Bridge) */
				Package (0x04) { 0x0009FFFF, 0x00, LNKF, 0x00 },

				/* NIC B (Bridge) */
				Package (0x04) { 0x000AFFFF, 0x00, LNKG, 0x00 },

				/* PCI-E Slot 4 (Bridge) */
				Package (0x04) { 0x000BFFFF, 0x00, LNKG, 0x00 },

				/* Bottom southbridge device (SP5100) */
				/* SATA 0 */
				Package (0x04) { 0x0011FFFF, 0x00, LNKG, 0x00 },

				/* USB 0 */
				Package (0x04) { 0x0012FFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0x0012FFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0x0012FFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0x0012FFFF, 0x03, LNKD, 0x00 },

				/* USB 1 */
				Package (0x04) { 0x0013FFFF, 0x00, LNKC, 0x00 },
				Package (0x04) { 0x0013FFFF, 0x01, LNKD, 0x00 },
				Package (0x04) { 0x0013FFFF, 0x02, LNKA, 0x00 },
				Package (0x04) { 0x0013FFFF, 0x03, LNKB, 0x00 },

				/* SMBUS / IDE / LPC / VGA / PCI Slots 0 - 2 */
				Package (0x04) { 0x0014FFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0x0014FFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0x0014FFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0x0014FFFF, 0x03, LNKD, 0x00 },
			})

			Name (AR00, Package () {
				/* APIC */
				/* Top southbridge device (SR5670) */
				/* HT Link */
				Package (0x04) { 0x0000FFFF, 0x00, 0x00, 55 },

				/* PCI-E Slot 1 (Bridge) */
				Package (0x04) { 0x0002FFFF, 0x00, 0x00, 52 },

				/* NIC A (Bridge) */
				Package (0x04) { 0x0009FFFF, 0x00, 0x00, 53 },

				/* NIC B (Bridge) */
				Package (0x04) { 0x000AFFFF, 0x00, 0x00, 54 },

				/* PCI-E Slot 4 (Bridge) */
				Package (0x04) { 0x000BFFFF, 0x00, 0x00, 54 },

				/* Bottom southbridge device (SP5100) */
				/* SATA 0 */
				Package (0x04) { 0x0011FFFF, 0x00, 0x00, 22 },

				/* USB 0 */
				Package (0x04) { 0x0012FFFF, 0x00, 0x00, 16 },
				Package (0x04) { 0x0012FFFF, 0x01, 0x00, 17 },
				Package (0x04) { 0x0012FFFF, 0x02, 0x00, 18 },
				Package (0x04) { 0x0012FFFF, 0x03, 0x00, 19 },

				/* USB 1 */
				Package (0x04) { 0x0013FFFF, 0x00, 0x00, 18 },
				Package (0x04) { 0x0013FFFF, 0x01, 0x00, 19 },
				Package (0x04) { 0x0013FFFF, 0x02, 0x00, 16 },
				Package (0x04) { 0x0013FFFF, 0x03, 0x00, 17 },

				/* SMBUS / IDE / LPC / VGA / PCI Slots 0 - 2 */
				Package (0x04) { 0x0014FFFF, 0x00, 0x00, 16 },
				Package (0x04) { 0x0014FFFF, 0x01, 0x00, 17 },
				Package (0x04) { 0x0014FFFF, 0x02, 0x00, 18 },
				Package (0x04) { 0x0014FFFF, 0x03, 0x00, 19 },
			})

			Name (PR01, Package () {
				/* PIC */
				Package (0x04) { 0x1FFFF, 0x00, LNKE, 0x00 },
				Package (0x04) { 0x1FFFF, 0x01, LNKF, 0x00 },
				Package (0x04) { 0x1FFFF, 0x02, LNKG, 0x00 },
				Package (0x04) { 0x1FFFF, 0x03, LNKH, 0x00 },
				Package (0x04) { 0x2FFFF, 0x00, LNKF, 0x00 },
				Package (0x04) { 0x2FFFF, 0x01, LNKG, 0x00 },
				Package (0x04) { 0x2FFFF, 0x02, LNKH, 0x00 },
				Package (0x04) { 0x2FFFF, 0x03, LNKE, 0x00 },
				Package (0x04) { 0x3FFFF, 0x00, LNKG, 0x00 },
				Package (0x04) { 0x3FFFF, 0x01, LNKH, 0x00 },
				Package (0x04) { 0x3FFFF, 0x02, LNKE, 0x00 },
				Package (0x04) { 0x3FFFF, 0x03, LNKF, 0x00 },
				Package (0x04) { 0x5FFFF, 0x00, LNKH, 0x00 },
			})

			Name (AR01, Package () {
				/* APIC */
				Package (0x04) { 0x1FFFF, 0x00, 0x00, 20 },
				Package (0x04) { 0x1FFFF, 0x01, 0x00, 21 },
				Package (0x04) { 0x1FFFF, 0x02, 0x00, 22 },
				Package (0x04) { 0x1FFFF, 0x03, 0x00, 23 },
				Package (0x04) { 0x2FFFF, 0x00, 0x00, 21 },
				Package (0x04) { 0x2FFFF, 0x01, 0x00, 22 },
				Package (0x04) { 0x2FFFF, 0x02, 0x00, 23 },
				Package (0x04) { 0x2FFFF, 0x03, 0x00, 20 },
				Package (0x04) { 0x3FFFF, 0x00, 0x00, 22 },
				Package (0x04) { 0x3FFFF, 0x01, 0x00, 23 },
				Package (0x04) { 0x3FFFF, 0x02, 0x00, 20 },
				Package (0x04) { 0x3FFFF, 0x03, 0x00, 21 },
				Package (0x04) { 0x5FFFF, 0x00, 0x00, 23 },
			})

			Name (PR02, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKD, 0x00 },
			})

			Name (AR02, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, 0x00, 24 },
				Package (0x04) { 0xFFFF, 0x01, 0x00, 25 },
				Package (0x04) { 0xFFFF, 0x02, 0x00, 26 },
				Package (0x04) { 0xFFFF, 0x03, 0x00, 27 },
			})

			Name (PR03, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKE, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKF, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKG, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKH, 0x00 },
			})

			Name (AR03, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, 0x00, 44 },
				Package (0x04) { 0xFFFF, 0x01, 0x00, 45 },
				Package (0x04) { 0xFFFF, 0x02, 0x00, 46 },
				Package (0x04) { 0xFFFF, 0x03, 0x00, 47 },
			})

			Name (PR04, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKD, 0x00 },
			})

			Name (AR04, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, 0x00, 48 },
				Package (0x04) { 0xFFFF, 0x01, 0x00, 49 },
				Package (0x04) { 0xFFFF, 0x02, 0x00, 50 },
				Package (0x04) { 0xFFFF, 0x03, 0x00, 51 },
			})

			Name (PR05, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKH, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKE, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKF, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKG, 0x00 },
			})

			Name (AR05, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, 0x00, 47 },
				Package (0x04) { 0xFFFF, 0x01, 0x00, 44 },
				Package (0x04) { 0xFFFF, 0x02, 0x00, 45 },
				Package (0x04) { 0xFFFF, 0x03, 0x00, 46 },
			})

			Name (PR06, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKD, 0x00 },
			})

			Name (AR06, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, 0x00, 32 },
				Package (0x04) { 0xFFFF, 0x01, 0x00, 33 },
				Package (0x04) { 0xFFFF, 0x02, 0x00, 34 },
				Package (0x04) { 0xFFFF, 0x03, 0x00, 35 },
			})

			Name (PR07, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKE, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKF, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKG, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKH, 0x00 },
			})

			Name (AR07, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, 0x00, 36 },
				Package (0x04) { 0xFFFF, 0x01, 0x00, 37 },
				Package (0x04) { 0xFFFF, 0x02, 0x00, 38 },
				Package (0x04) { 0xFFFF, 0x03, 0x00, 39 },
			})

			Name (PR08, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKD, 0x00 },
			})

			Name (AR08, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, 0x00, 40 },
				Package (0x04) { 0xFFFF, 0x01, 0x00, 41 },
				Package (0x04) { 0xFFFF, 0x02, 0x00, 42 },
				Package (0x04) { 0xFFFF, 0x03, 0x00, 43 },
			})

			/* PCI Resource Tables */

			/* PCI Resource Settings Access */
			Method (_CRS, 0, Serialized)
			{
				Name (BUF0, ResourceTemplate ()
				{
					IO (Decode16,
					0x0CF8,	// Address Range Minimum
					0x0CF8,	// Address Range Maximum
					0x01,	// Address Alignment
					0x08,	// Address Length
					)
					WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,	// Address Space Granularity
					0x0000,	// Address Range Minimum
					0x0CF7,	// Address Range Maximum
					0x0000,	// Address Translation Offset
					0x0CF8,	// Address Length
					,, , TypeStatic)
				})
				/* Methods below use SSDT to get actual MMIO regs
				   The IO ports are from 0xd00, optionally an VGA,
				   otherwise the info from MMIO is used.
				   \_SB.GXXX(node, link)
				 */
				Concatenate (\_SB.GMEM (0x00, \_SB.PCI0.SBLK), BUF0, Local1)
				Concatenate (\_SB.GIOR (0x00, \_SB.PCI0.SBLK), Local1, Local2)
				Concatenate (\_SB.GWBN (0x00, \_SB.PCI0.SBLK), Local2, Local3)
				Return (Local3)
			}

			/* PCI Routing Table Access */
			Method (_PRT, 0, NotSerialized) {
				If (PICM) {
					Return (AR00)
				} Else {
					Return (PR00)
				}
			}

			/* 0:11.0 SP5100 SATA 0 */
			Device(SAT0)
			{
				Name (_ADR, 0x00110000)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
				#include <southbridge/amd/sb700/acpi/sata.asl>
			}

			/* 0:12.0 SP5100 USB 0 */
			Device (USB0)
			{
				Name (_ADR, 0x00120000)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
			}

			/* 0:12.1 SP5100 USB 1 */
			Device (USB1)
			{
				Name (_ADR, 0x00120001)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
			}

			/* 0:12.2 SP5100 USB 2 */
			Device (USB2)
			{
				Name (_ADR, 0x00120002)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
			}

			/* 0:13.0 SP5100 USB 3 */
			Device (USB3)
			{
				Name (_ADR, 0x00130000)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
			}

			/* 0:13.1 SP5100 USB 4 */
			Device (USB4)
			{
				Name (_ADR, 0x00130001)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
			}

			/* 0:13.2 SP5100 USB 5 */
			Device (USB5)
			{
				Name (_ADR, 0x00130002)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
			}

			/* 0:14.1 SP5100 IDE Controller */
			Device (IDEC)
			{
				Name (_ADR, 0x00140001)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
				#include <southbridge/amd/sb700/acpi/ide.asl>
			}

			/* 0:14.3 SP5100 LPC */
			Device (LPC) {
				Name (_HID, EisaId ("PNP0A05"))
				Name (_ADR, 0x00140003)

				/* Real Time Clock Device */
				Device(RTC0) {
					Name(_HID, EISAID("PNP0B00"))   /* AT Real Time Clock (not PIIX4 compatible) */
					Name(BUF0, ResourceTemplate() {
						IO(Decode16, 0x0070, 0x0070, 0x01, 0x02)
					})
					Name(BUF1, ResourceTemplate() {
						IRQNoFlags() { 8 }
						IO(Decode16, 0x0070, 0x0070, 0x01, 0x02)
					})
					Method(_CRS, 0) {
						If(HPTE) {
							Return(BUF0)
						}
						Return(BUF1)
					}
				}

				Device(TMR) {   /* Timer */
					Name(_HID,EISAID("PNP0100"))    /* System Timer */
					Name(BUF0, ResourceTemplate() {
						IO(Decode16, 0x0040, 0x0040, 0x01, 0x04)
					})
					Name(BUF1, ResourceTemplate() {
						IRQNoFlags() { 0 }
						IO(Decode16, 0x0040, 0x0040, 0x01, 0x04)
					})
					Method(_CRS, 0) {
						If(HPTE) {
							Return(BUF0)
						}
						Return(BUF1)
					}
				}

				Device(SPKR) {  /* Speaker */
					Name(_HID,EISAID("PNP0800"))    /* AT style speaker */
					Name(_CRS, ResourceTemplate() {
						IO(Decode16, 0x0061, 0x0061, 0, 1)
					})
				}

				Device(PIC) {
					Name(_HID,EISAID("PNP0000"))    /* AT Interrupt Controller */
					Name(_CRS, ResourceTemplate() {
						IRQNoFlags() { 2 }
						IO(Decode16,0x0020, 0x0020, 0, 2)
						IO(Decode16,0x00A0, 0x00A0, 0, 2)
					})
				}

				Device(MAD) { /* 8257 DMA */
					Name(_HID,EISAID("PNP0200"))    /* Hardware Device ID */
					Name(_CRS, ResourceTemplate() {
						DMA(Compatibility,BusMaster,Transfer8){4}
						IO(Decode16, 0x0000, 0x0000, 0x10, 0x10)
						IO(Decode16, 0x0081, 0x0081, 0x01, 0x03)
						IO(Decode16, 0x0087, 0x0087, 0x01, 0x01)
						IO(Decode16, 0x0089, 0x0089, 0x01, 0x03)
						IO(Decode16, 0x008F, 0x008F, 0x01, 0x01)
						IO(Decode16, 0x00C0, 0x00C0, 0x10, 0x20)
					}) /* End Name(_SB.PCI0.LpcIsaBr.MAD._CRS) */
				}

				Device(COPR) {
					Name(_HID,EISAID("PNP0C04"))    /* Math Coprocessor */
					Name(_CRS, ResourceTemplate() {
						IO(Decode16, 0x00F0, 0x00F0, 0, 0x10)
						IRQNoFlags(){13}
					})
				}

				#include "../../../superio/winbond/w83667hg-a/ps2_controller.asl"

				/* UART 1 */
				Device (URT1)
				{
					Name (_HID, EisaId ("PNP0501"))		// "PNP0501" for UART
					Name(_PRW, Package () {0x03, 0x04})	// Wake from S1-S4
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0f)			// Always enable
					}
					Name (_PRS, ResourceTemplate() {
						StartDependentFn(0, 1) {
							IO(Decode16, 0x3f8, 0x3f8, 0x8, 0x8)
							IRQNoFlags() { 4 }
						} EndDependentFn()
					})
					Method (_CRS, 0)
					{
						Return(ResourceTemplate() {
							IO(Decode16, 0x3f8, 0x3f8, 0x8, 0x8)
							IRQNoFlags() { 4 }
						})
					}
				}

				/* UART 2 */
				Device (URT2)
				{
					Name (_HID, EisaId ("PNP0501"))		// "PNP0501" for UART
					Name(_PRW, Package () {0x03, 0x04})	// Wake from S1-S4
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0f)			// Always enable
					}
					Name (_PRS, ResourceTemplate() {
						StartDependentFn(0, 1) {
							IO(Decode16, 0x2f8, 0x2f8, 0x8, 0x8)
							IRQNoFlags() { 3 }
						} EndDependentFn()
					})
					Method (_CRS, 0)
					{
						Return(ResourceTemplate() {
							IO(Decode16, 0x2f8, 0x2f8, 0x8, 0x8)
							IRQNoFlags() { 3 }
						})
					}
				}
			}

			/* High Precision Event Timer */
			Device (HPET)
			{
				Name (_HID, EisaId ("PNP0103"))
				Name (CRS, ResourceTemplate ()
				{
					Memory32Fixed(ReadOnly, 0xFED00000, 0x00000400)
				})
				Method (_STA, 0)
				{
					If(HPTE) {
						Return (0x0F)
					}
					Return (0x0)
				}
				Method(_CRS, 0)
				{
					Return(CRS)
				}
			}

			/* 0:14.4 PCI Bridge */
			Device (PBR0)
			{
				Name (_ADR, 0x00140004)			// _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)		// _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR01)
					} Else {
						Return (PR01)
					}
				}
				Device (SLT1)
				{
					Name (_ADR, 0xFFFF)			// _ADR: Address
					Name(_PRW, Package () {0x0B, 0x04})	// Wake from S1-S4
				}
			}

			/* 0:14.5 SP5100 USB 6 */
			Device (USB6)
			{
				Name (_ADR, 0x00140005)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
			}

			/* 2:00.0 PCIe x16 */
			Device (PCE1)
			{
				Name (_ADR, 0x00020000)			// _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)		// _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR02)
					} Else {
						Return (PR02)
					}
				}
				Device (SLT1)
				{
					Name (_ADR, 0xFFFF)			// _ADR: Address
					Name(_PRW, Package () {0x0B, 0x04})	// Wake from S1-S4
				}
			}

			/* 1:00.0 PIKE */
			Device (PIKE)
			{
				Name (_ADR, 0x00040000)			// _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)		// _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR03)
					} Else {
						Return (PR03)
					}
				}
				Device (SLT1)
				{
					Name (_ADR, 0xFFFF)			// _ADR: Address
					Name(_PRW, Package () {0x0B, 0x04})	// Wake from S1-S4
				}
			}

			/* 3:00.0 PCIe NIC A */
			Device (NICA)
			{
				Name (_ADR, 0x00090000)  // _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR04)
					} Else {
						Return (PR04)
					}
				}
				Device (BDC1)
				{
					Name (_ADR, Zero)  // _ADR: Address
				}
			}

			/* 4:00.0 PCIe NIC B */
			Device (NICB)
			{
				Name (_ADR, 0x000A0000)  // _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR05)
					} Else {
						Return (PR05)
					}
				}
				Device (BDC2)
				{
					Name (_ADR, Zero)  // _ADR: Address
				}
			}

			/* 5:00.0 PCIe x16 */
			Device (PCE4)
			{
				Name (_ADR, 0x000B0000)			// _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)		// _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR06)
					} Else {
						Return (PR06)
					}
				}
				Device (SLT1)
				{
					Name (_ADR, 0xFFFF)			// _ADR: Address
					Name(_PRW, Package () {0x0B, 0x04})	// Wake from S1-S4
				}
			}
		}

		Device (PWRB) {	/* Start Power button device */
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_PRW, Package () {3, 0x04})	/* wake from S1-S4 */
			Name(_STA, 0x0B) /* sata is invisible */
		}
	}

#include "acpi/pm_ctrl.asl"

}
