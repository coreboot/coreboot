/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
		0x02,		/* DSDT Revision, needs to be 2 for 64bit */
		"ASUS  ",	/* OEMID */
		"COREBOOT",	/* TABLE ID */
		0x00000001	/* OEM Revision */
		)
{
	#include "northbridge/amd/amdfam10/amdfam10_util.asl"

	/* Some global data */
	Name(OSVR, 3)	/* Assume nothing. WinXp = 1, Vista = 2, Linux = 3, WinCE = 4 */
	Name(OSV, Ones)	/* Assume nothing */
	Name(PICM, One)	/* Assume APIC */

	/* HPET control */
	Name (SHPB, 0xFED00000)
	Name (SHPL, 0x1000)

	/* Define power states */
	Name (\_S0, Package () { 0x00, 0x00, 0x00, 0x00 })	/* Normal operation */
	Name (_S1, Package () { 0x01, 0x00, 0x00, 0x00 })	/* Standby */
	/* Name (_S3, Package () { 0x05, 0x00, 0x00, 0x00 }) */	/* Not supported by hardware */
	/* Name (_S4, Package () { 0x06, 0x00, 0x00, 0x00 }) */
	Name (\_S5, Package () { 0x07, 0x00, 0x00, 0x00 })	/* Hard power off */

	/* The _PIC method is called by the OS to choose between interrupt
		* routing via the i8259 interrupt controller or the APIC.
		*
		* _PIC is called with a parameter of 0 for i8259 configuration and
		* with a parameter of 1 for Local Apic/IOAPIC configuration.
		*/
	Method (_PIC, 1, Serialized) {
		Store (Arg0, PICM)
	}

	/* _PR CPU0 is dynamically supplied by SSDT */
	/* CPU objects and _PSS entries are dynamically supplied by SSDT */

	Scope(\_GPE) {	/* Start Scope GPE */
		/*  VGA controller PME#  */
		Method(_L00) {
			/* Level-Triggered GPE */
			Notify(\_SB.PCI0.VGAC, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02)			/* NOTIFY_DEVICE_WAKE */
		}

		/*  Keyboard controller PME#  */
		Method(_L03) {
			/* Level-Triggered GPE */
			Notify(\_SB.PCI0.LPC.KBD, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.LPC.MOU, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02)			/* NOTIFY_DEVICE_WAKE */
		}

		/*  USB2 PME#  */
		Method(_L05) {
			/* Level-Triggered GPE */
			Notify (\_SB.PCI0.USB2, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PWRB, 0x02)		/* NOTIFY_DEVICE_WAKE */
		}

		/*  Slot PME#  */
		Method(_L0B) {
			/* Level-Triggered GPE */
			Notify (\_SB.PCI0.PCIE.SLT1, 0x02)	/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.LSIC.SLT2, 0x02)	/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PWRB, 0x02)		/* NOTIFY_DEVICE_WAKE */
		}

		/*  USB0 PME#  */
		Method(_L0D) {
			/* Level-Triggered GPE */
			Notify (\_SB.PCI0.USB0, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PWRB, 0x02)		/* NOTIFY_DEVICE_WAKE */
		}

		/*  Keyboard controller PME#  */
		Method(_L10) {
			/* Level-Triggered GPE */
			Notify(\_SB.PCI0.LPC.KBD, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.LPC.MOU, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02)			/* NOTIFY_DEVICE_WAKE */
		}

		/*  PCIe PME#  */
		Method(_L11) {
			/* Level-Triggered GPE */
			Notify (\_SB.PCI0.NICB, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.PCIE, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.NICA, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PCI0.LSIC, 0x02)		/* NOTIFY_DEVICE_WAKE */
			Notify (\_SB.PWRB, 0x02)		/* NOTIFY_DEVICE_WAKE */
		}

	}	/* End Scope GPE */

	/* Root of the bus hierarchy */
	Scope (\_SB)
	{
		/* Top PCI device (CK804) */
		Device (PCI0)
		{
			/* BUS0 root bus */

			Name (_HID, EisaId ("PNP0A03"))
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
				/* ISA Bridge */
				Package (0x04) { 0x0001FFFF, 0x00, LKSM, 0x00 },

				/* USB */
				Package (0x04) { 0x0002FFFF, 0x00, LUB0, 0x00 },
				Package (0x04) { 0x0002FFFF, 0x01, LUB2, 0x00 },

				/* SATA 0 */
				Package (0x04) { 0x0007FFFF, 0x00, LSA0, 0x00 },

				/* SATA 1 */
				Package (0x04) { 0x0008FFFF, 0x00, LSA1, 0x00 },

				/* NIC A (Bridge) */
				Package (0x04) { 0x000BFFFF, 0x00, LNKB, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x01, LNKC, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x02, LNKD, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x03, LNKA, 0x00 },

				/* NIC B (Bridge) */
				Package (0x04) { 0x000CFFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x03, LNKD, 0x00 },

				/* LSI SAS Controller (Bridge) */
				Package (0x04) { 0x000DFFFF, 0x00, LNKD, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x01, LNKA, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x02, LNKB, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x03, LNKC, 0x00 },

				/* PCI-E Slot (Bridge) */
				Package (0x04) { 0x000EFFFF, 0x00, LNKC, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x01, LNKD, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x02, LNKA, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x03, LNKB, 0x00 },
			})

			Name (AR00, Package () {
				/* APIC */
				/* ISA Bridge */
				Package (0x04) { 0x0001FFFF, 0x00, LKSM, 0x00 },

				/* USB */
				Package (0x04) { 0x0002FFFF, 0x00, LUB0, 0x00 },
				Package (0x04) { 0x0002FFFF, 0x01, LUB2, 0x00 },

				/* SATA 0 */
				Package (0x04) { 0x0007FFFF, 0x00, LSA0, 0x00 },

				/* SATA 1 */
				Package (0x04) { 0x0008FFFF, 0x00, LSA1, 0x00 },

				/* NIC A (Bridge) */
				Package (0x04) { 0x000BFFFF, 0x00, LNIB, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x01, LNIC, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x02, LNND, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x03, LNIA, 0x00 },

				/* NIC B (Bridge) */
				Package (0x04) { 0x000CFFFF, 0x00, LNIA, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x01, LNIB, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x02, LNIC, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x03, LNND, 0x00 },

				/* LSI SAS Controller (Bridge) */
				Package (0x04) { 0x000DFFFF, 0x00, LNND, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x01, LNIA, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x02, LNIB, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x03, LNIC, 0x00 },

				/* PCI-E Slot (Bridge) */
				Package (0x04) { 0x000EFFFF, 0x00, LNIC, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x01, LNND, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x02, LNIA, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x03, LNIB, 0x00 },
			})

			Name (PR01, Package () {
				/* PIC */
				Package (0x04) { 0x0004FFFF, 0x00, LNKA, 0x00 },
			})

			Name (AR01, Package () {
				/* APIC */
				Package (0x04) { 0x0004FFFF, 0x00, LNIA, 0x00 },
			})

			Name (PR02, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKB, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKC, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKD, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKA, 0x00 },
			})

			Name (AR02, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, LNIB, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNIC, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNND, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNIA, 0x00 },
			})

			Name (PR03, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKD, 0x00 },
			})

			Name (AR03, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, LNIA, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNIB, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNIC, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNND, 0x00 },
			})

			Name (PR04, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKD, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKA, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKB, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKC, 0x00 },
			})

			Name (AR04, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, LNND, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNIA, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNIB, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNIC, 0x00 },
			})

			Name (PR05, Package () {
				/* PIC */
				Package (0x04) { 0xFFFF, 0x00, LNKC, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNKD, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNKA, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNKB, 0x00 },
			})

			Name (AR05, Package () {
				/* APIC */
				Package (0x04) { 0xFFFF, 0x00, LNIC, 0x00 },
				Package (0x04) { 0xFFFF, 0x01, LNND, 0x00 },
				Package (0x04) { 0xFFFF, 0x02, LNIA, 0x00 },
				Package (0x04) { 0xFFFF, 0x03, LNIB, 0x00 },
			})

			/* PCI Resource Tables */

			Name (RSIA, ResourceTemplate () {
				/* PIC */
				IRQ (Level, ActiveLow, Shared, ) {8}
			})

			Name (RSMA, ResourceTemplate () {
				/* APIC */
				Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, ) {16}
			})

			Name (RSIB, ResourceTemplate () {
				/* PIC */
				IRQ (Level, ActiveLow, Shared, ) {1}
			})

			Name (RSMB, ResourceTemplate () {
				/* APIC */
				Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, ) {17}
			})

			Name (RSIC, ResourceTemplate () {
				/* PIC */
				IRQ (Level, ActiveLow, Shared, ) {2}
			})

			Name (RSMC, ResourceTemplate () {
				/* APIC */
				Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, ) {18}
			})

			Name (RSND, ResourceTemplate () {
				/* PIC */
				IRQ (Level, ActiveLow, Shared, ) {13}
			})

			Name (RSMD, ResourceTemplate () {
				/* APIC */
				Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, ) {19}
			})

			Name (RSS2, ResourceTemplate ()
			{
				/* PIC */
				IRQ (Level, ActiveLow, Shared, )
					{3, 4, 5, 7, 9, 10, 11, 12, 14, 15}
			})

			Name (RSA1, ResourceTemplate ()
			{
				/* APIC */
				IRQ (Level, ActiveLow, Shared, )
					{3, 4, 5, 6, 7, 10, 11, 12, 14, 15}
			})

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

#include "southbridge/nvidia/ck804/acpi/ck804.asl"

			/* PCI Routing Table Access */
			Method (_PRT, 0, NotSerialized) {
				If (PICM) {
					Return (AR00)
				} Else {
					Return (PR00)
				}
			}

			/* USB0 */
			Device (LUB0)
			{
				Name (_HID, EisaId ("PNP0C0F"))  // _HID: Hardware ID
				Name (_UID, 0x05)  // _UID: Unique ID

				Method (_STA, 0, Serialized) {
					If (\_SB.PCI0.LPCB.INTQ) {
						Return (0xb)
					} Else {
						Return (0x9)
					}
				}
				Method (_DIS, 0, Serialized) {
					Store (0, \_SB.PCI0.LPCB.INTQ)
				}
				Method (_PRS, 0, Serialized) {
					If (PICM) {
						Return (PRSC)
					} Else {
						Return (RSA1)
					}
				}
				Method (_CRS, 0, Serialized) {
					If (PICM) {
						Return (CRSC(\_SB.PCI0.LPCB.INTQ))
					} Else {
						Return (CRSA(\_SB.PCI0.LPCB.INTQ))
					}
				}
				Method (_SRS, 1, Serialized) {
					If (PICM) {
						Store (SRSC(Arg0), \_SB.PCI0.LPCB.INTQ)
					} Else {
						Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTQ)
					}
				}
			}

			/* USB2 */
			Device (LUB2)
			{
				Name (_HID, EisaId ("PNP0C0F"))  // _HID: Hardware ID
				Name (_UID, 0x07)  // _UID: Unique ID

				Method (_STA, 0, Serialized) {
					If (\_SB.PCI0.LPCB.INTL) {
						Return (0xb)
					} Else {
						Return (0x9)
					}
				}
				Method (_DIS, 0, Serialized) {
					Store (0, \_SB.PCI0.LPCB.INTL)
				}
				Method (_PRS, 0, Serialized) {
					If (PICM) {
						Return (PRSC)
					} Else {
						Return (RSA1)
					}
				}
				Method (_CRS, 0, Serialized) {
					If (PICM) {
						Return (CRSC(\_SB.PCI0.LPCB.INTL))
					} Else {
						Return (CRSA(\_SB.PCI0.LPCB.INTL))
					}
				}
				Method (_SRS, 1, Serialized) {
					If (PICM) {
						Store (SRSC(Arg0), \_SB.PCI0.LPCB.INTL)
					} Else {
						Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTL)
					}
				}
			}

			/* ISA Bridge */
			Device (LKSM)
			{
				Name (_HID, EisaId ("PNP0C0F"))  // _HID: Hardware ID
				Name (_UID, 0x0C)  // _UID: Unique ID

				Method (_STA, 0, Serialized) {
					If (\_SB.PCI0.LPCB.INTK) {
						Return (0xb)
					} Else {
						Return (0x9)
					}
				}
				Method (_DIS, 0, Serialized) {
					Store (0, \_SB.PCI0.LPCB.INTK)
				}
				Method (_PRS, 0, Serialized) {
					If (PICM) {
						Return (RSA1)
					} Else {
						Return (RSS2)
					}
				}
				Method (_CRS, 0, Serialized) {
					If (PICM) {
						Return (CRSB(\_SB.PCI0.LPCB.INTK))
					} Else {
						Return (CRSA(\_SB.PCI0.LPCB.INTK))
					}
				}
				Method (_SRS, 1, Serialized) {
					If (PICM) {
						Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTK)
					} Else {
						Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTK)
					}
				}
			}

			/* Bridge device link (NIC A) */
			Device (LNIA)
			{
				Name (_HID, EisaId ("PNP0C0F"))  // _HID: Hardware ID
				Name (_UID, 0x10)  // _UID: Unique ID

				Method (_STA, 0, Serialized) {
					If (\_SB.PCI0.LPCB.INTA) {
						Return (0xb)
					} Else {
						Return (0x9)
					}
				}
				Method (_DIS, 0, Serialized) {
					Store (0, \_SB.PCI0.LPCB.INTA)
				}
				Method (_PRS, 0, Serialized) {
					If (PICM) {
						Return (RSMA)
					} Else {
						Return (RSIA)
					}
				}
				Method (_CRS, 0, Serialized) {
					If (PICM) {
						Return (CRSB(\_SB.PCI0.LPCB.INTA))
					} Else {
						Return (CRSA(\_SB.PCI0.LPCB.INTA))
					}
				}
				Method (_SRS, 1, Serialized) {
					If (PICM) {
						Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTA)
					} Else {
						Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTA)
					}
				}
			}

			/* Bridge device link (NIC B) */
			Device (LNIB)
			{
				Name (_HID, EisaId ("PNP0C0F"))  // _HID: Hardware ID
				Name (_UID, 0x11)  // _UID: Unique ID

				Method (_STA, 0, Serialized) {
					If (\_SB.PCI0.LPCB.INTB) {
						Return (0xb)
					} Else {
						Return (0x9)
					}
				}
				Method (_DIS, 0, Serialized) {
					Store (0, \_SB.PCI0.LPCB.INTB)
				}
				Method (_PRS, 0, Serialized) {
					If (PICM) {
						Return (RSMB)
					} Else {
						Return (RSIB)
					}
				}
				Method (_CRS, 0, Serialized) {
					If (PICM) {
						Return (CRSB(\_SB.PCI0.LPCB.INTB))
					} Else {
						Return (CRSA(\_SB.PCI0.LPCB.INTB))
					}
				}
				Method (_SRS, 1, Serialized) {
					If (PICM) {
						Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTB)
					} Else {
						Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTB)
					}
				}
			}

			/* Bridge device link */
			Device (LNIC)
			{
				Name (_HID, EisaId ("PNP0C0F"))  // _HID: Hardware ID
				Name (_UID, 0x12)  // _UID: Unique ID

				Method (_STA, 0, Serialized) {
					If (\_SB.PCI0.LPCB.INTC) {
						Return (0xb)
					} Else {
						Return (0x9)
					}
				}
				Method (_DIS, 0, Serialized) {
					Store (0, \_SB.PCI0.LPCB.INTC)
				}
				Method (_PRS, 0, Serialized) {
					If (PICM) {
						Return (RSMC)
					} Else {
						Return (RSIC)
					}
				}
				Method (_CRS, 0, Serialized) {
					If (PICM) {
						Return (CRSB(\_SB.PCI0.LPCB.INTC))
					} Else {
						Return (CRSA(\_SB.PCI0.LPCB.INTC))
					}
				}
				Method (_SRS, 1, Serialized) {
					If (PICM) {
						Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTC)
					} Else {
						Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTC)
					}
				}
			}

			/* Bridge device link */
			Device (LNND)
			{
				Name (_HID, EisaId ("PNP0C0F"))  // _HID: Hardware ID
				Name (_UID, 0x13)  // _UID: Unique ID

				Method (_STA, 0, Serialized) {
					If (\_SB.PCI0.LPCB.INTD) {
						Return (0xb)
					} Else {
						Return (0x9)
					}
				}
				Method (_DIS, 0, Serialized) {
					Store (0, \_SB.PCI0.LPCB.INTD)
				}
				Method (_PRS, 0, Serialized) {
					If (PICM) {
						Return (RSMD)
					} Else {
						Return (RSND)
					}
				}
				Method (_CRS, 0, Serialized) {
					If (PICM) {
						Return (CRSB(\_SB.PCI0.LPCB.INTD))
					} Else {
						Return (CRSA(\_SB.PCI0.LPCB.INTD))
					}
				}
				Method (_SRS, 1, Serialized) {
					If (PICM) {
						Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTD)
					} Else {
						Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTD)
					}
				}
			}

			/* 0:02.0 CK804 USB 0 */
			Device (USB0)
			{
				Name (_ADR, 0x00020000)  // _ADR: Address
				Name(_PRW, Package () {0x0D, 0x04})	// Wake from S1-S4
			}

			/* 0:02.0 CK804 USB 2 */
			Device (USB2)
			{
				Name (_ADR, 0x00020001)  // _ADR: Address
				Name(_PRW, Package () {0x05, 0x04})	// Wake from S1-S4
			}

			/* 1:04.0 VGA Controller */
			Device (VGAC)
			{
				Name (_ADR, 0x00090000)  // _ADR: Address
				Name(_PRW, Package () {0x00, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR01)
					} Else {
						Return (PR01)
					}
				}
			}

			/* 2:00.0 PCIe NIC A */
			Device (NICA)
			{
				Name (_ADR, 0x000B0000)  // _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR02)
					} Else {
						Return (PR02)
					}
				}
				Device (BDC1)
				{
					Name (_ADR, Zero)  // _ADR: Address
				}
			}

			/* 3:00.0 PCIe NIC B */
			Device (NICB)
			{
				Name (_ADR, 0x000C0000)  // _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR03)
					} Else {
						Return (PR03)
					}
				}
				Device (BDC2)
				{
					Name (_ADR, Zero)  // _ADR: Address
				}
			}

			/* 4:00.0 PCIe LSI SAS Controller */
			Device (LSIC)
			{
				Name (_ADR, 0x000D0000)  // _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR04)
					} Else {
						Return (PR04)
					}
				}

				Device (SLT2)
				{
					Name (_ADR, 0xFFFF)  // _ADR: Address
					Name(_PRW, Package () {0x0B, 0x04})	// Wake from S1-S4
				}
			}

			/* 5:00.0 PCIe x16 */
			Device (PCIE)
			{
				Name (_ADR, 0x000E0000)			// _ADR: Address
				Name(_PRW, Package () {0x11, 0x04})	// Wake from S1-S4
				Method (_PRT, 0, NotSerialized)		// _PRT: PCI Routing Table
				{
					If (PICM) {
						Return (AR05)
					} Else {
						Return (PR05)
					}
				}
				Device (SLT1)
				{
					Name (_ADR, 0xFFFF)			// _ADR: Address
					Name(_PRW, Package () {0x0B, 0x04})	// Wake from S1-S4
				}
			}

			Device (LPC) {
				Name (_HID, EisaId ("PNP0A05"))
				Name (_ADR, 0x00010000)

				/* PS/2 keyboard (seems to be important for WinXP install) */
				Device (KBD)
				{
					Name (_HID, EisaId ("PNP0303"))
					Name (_CID, EisaId ("PNP030B"))
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0f)
					}
					Method (_CRS, 0, Serialized)
					{
						Name (TMP, ResourceTemplate () {
							IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
							IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
							IRQNoFlags () {1}
						})
						Return (TMP)
					}
				}

				/* PS/2 mouse */
				Device (MOU)
				{
					Name (_HID, EisaId ("PNP0F03"))
					Name (_CID, EisaId ("PNP0F13"))
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0f)
					}
					Method (_CRS, 0, Serialized)
					{
						Name (TMP, ResourceTemplate () {
							IRQNoFlags () {12}
						})
						Return (TMP)
					}
				}


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

				/* Floppy controller */
				Device (FDC0)
				{
					Name (_HID, EisaId ("PNP0700"))
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0f)
					}
					Method (_CRS, 0, Serialized)
					{
						Name (BUF0, ResourceTemplate () {
							FixedIO (0x03F0, 0x08)
							IRQNoFlags () {6}
							DMA (Compatibility, NotBusMaster, Transfer8) {2}
						})
						Return (BUF0)
					}
				}
				Device (HPET)
				{
					Name (_HID, EisaId ("PNP0103"))
					Name (CRS, ResourceTemplate ()
					{
						Memory32Fixed (ReadOnly,
						0x00000000,
						0x00001000,
						_Y02)
						IRQNoFlags () {0}
						IRQNoFlags () {8}
					})
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0F)
					}
					Method (_CRS, 0, NotSerialized)
					{
						CreateDWordField (CRS, \_SB.PCI0.LPC.HPET._Y02._BAS, HPT1)
						CreateDWordField (CRS, \_SB.PCI0.LPC.HPET._Y02._LEN, HPT2)
						Store (SHPB, HPT1)
						Store (SHPL, HPT2)
						Return (CRS)
					}

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
