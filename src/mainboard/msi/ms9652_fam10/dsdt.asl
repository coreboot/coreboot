/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <Nick.Barker9@btinternet.com>
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 Harald Gutmann <harald.gutmann@gmx.net>
 *
 * ISA portions taken from QEMU acpi-dsdt.dsl.
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

DefinitionBlock ("DSDT.aml", "DSDT", 1, "CORE  ", "COREBOOT", 1)
{
	#include "acpi/util.asl"

	/* For now only define 2 power states:
	 *  - S0 which is fully on
	 *  - S5 which is soft off
	 */
	Name (\_S0, Package () { 0x00, 0x00, 0x00, 0x00 })
	Name (\_S5, Package () { 0x07, 0x00, 0x00, 0x00 })

	/* Root of the bus hierarchy */
	Scope (\_SB)
	{
		/* Top PCI device */
		Device (PCI0)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00)
			Name (_UID, 0x00)
			Name (_BBN, 0x00)

			External (BUSN)
			External (MMIO)
			External (PCIO)
			External (SBLK)
			External (TOM1)
			External (HCLK)
			External (SBDN)
			External (HCDN)

			Method (_CRS, 0, NotSerialized)
			{
				Name (BUF0, ResourceTemplate ()
				{
					IO (Decode16,
					0x0CF8,             // Address Range Minimum
					0x0CF8,             // Address Range Maximum
					0x01,               // Address Alignment
					0x08,               // Address Length
					)
					WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,             // Address Space Granularity
					0x0000,             // Address Range Minimum
					0x0CF7,             // Address Range Maximum
					0x0000,             // Address Translation Offset
					0x0CF8,             // Address Length
					,, , TypeStatic)
				})
				/* Methods bellow use SSDT to get actual MMIO regs
				   The IO ports are from 0xd00, optionally an VGA,
				   otherwise the info from MMIO is used.
				 */
				Concatenate (\_SB.GMEM (0x00, \_SB.PCI0.SBLK), BUF0, Local1)
				Concatenate (\_SB.GIOR (0x00, \_SB.PCI0.SBLK), Local1, Local2)
				Concatenate (\_SB.GWBN (0x00, \_SB.PCI0.SBLK), Local2, Local3)
				Return (Local3)
			}

			/* PCI Routing Table */
			Name (_PRT, Package () {
				Package (0x04) { 0x0001FFFF, 0x01, 0x00, 0x0A }, /* 0x1 - 00:01.1 - IRQ 10 - SMBus */
				Package (0x04) { 0x0002FFFF, 0x00, 0x00, 0x16 }, /* 0x2 - 00:02.0 - IRQ 22 - USB */
				Package (0x04) { 0x0002FFFF, 0x01, 0x00, 0x17 }, /* 0x2 - 00:01.1 - IRQ 23 - USB */
				Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x15 }, /* 0x4 - 00:04.0 - IRQ 21 - IDE */
				Package (0x04) { 0x0005FFFF, 0x00, 0x00, 0x14 }, /* 0x5 - 00:05.0 - IRQ 20 - SATA */
				Package (0x04) { 0x0005FFFF, 0x01, 0x00, 0x15 }, /* 0x5 - 00:05.1 - IRQ 21 - SATA */
				Package (0x04) { 0x0005FFFF, 0x02, 0x00, 0x16 }, /* 0x5 - 00:05.2 - IRQ 22 - SATA */
				Package (0x04) { 0x0006FFFF, 0x01, 0x00, 0x17 }, /* 0x6 - 00:06.1 - IRQ 23 - HD Audio */
				Package (0x04) { 0x0008FFFF, 0x00, 0x00, 0x14 }, /* 0x8 - 00:08.0 - IRQ 20 - GBit Ethernet */
			})

			Device (PEBF) /* PCI-E Bridge F */
			{
				Name (_ADR, 0x000F0000)
				Name (_UID, 0x00)
				Name (_BBN, 0x07)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x11 },
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x12 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x13 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x10 },
				})
			}

			Device (PEBE) /* PCI-E Bridge E */
			{
				Name (_ADR, 0x000E0000)
				Name (_UID, 0x00)
				Name (_BBN, 0x06)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x12 },
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x13 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x10 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x11 },
				})
			}

			Device (PEBD) /* PCI-E Bridge D */
			{
				Name (_ADR, 0x000D0000)
				Name (_UID, 0x00)
				Name (_BBN, 0x05)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x13 },
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x10 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x11 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x12 },
				})
			}

			Device (PEBC) /* PCI-E Bridge C */
			{
				Name (_ADR, 0x000C0000)
				Name (_UID, 0x00)
				Name (_BBN, 0x04)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x10 },
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x11 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x12 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x13 },
				})
			}

			Device (PEBB) /* PCI-E Bridge B */
			{
				Name (_ADR, 0x000B0000)
				Name (_UID, 0x00)
				Name (_BBN, 0x03)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x11 },
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x12 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x13 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x10 },
				})
			}

			Device (PEBA) /* PCI-E Bridge A */
			{
				Name (_ADR, 0x000A0000)
				Name (_UID, 0x00)
				Name (_BBN, 0x02)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x12 },
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x13 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x10 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x11 },
				})
			}

			Device (PCID)	/* PCI Device */
			{
				Name (_ADR, 0x00060000)
				Name (_UID, 0x00)
				Name (_BBN, 0x01)
				Name (_PRT, Package () {
					Package (0x04) { 0x0006FFFF, 0x00, 0x00, 0x12 },
					Package (0x04) { 0x0006FFFF, 0x01, 0x00, 0x13 },
					Package (0x04) { 0x0006FFFF, 0x02, 0x00, 0x10 },
					Package (0x04) { 0x0006FFFF, 0x03, 0x00, 0x11 },
					Package (0x04) { 0x0007FFFF, 0x00, 0x00, 0x13 }, /* PCI slot 1 */
					Package (0x04) { 0x0007FFFF, 0x01, 0x00, 0x10 },
					Package (0x04) { 0x0007FFFF, 0x02, 0x00, 0x11 },
					Package (0x04) { 0x0007FFFF, 0x03, 0x00, 0x12 },
					Package (0x04) { 0x0008FFFF, 0x00, 0x00, 0x10 }, /* PCI slot 2 */
					Package (0x04) { 0x0008FFFF, 0x01, 0x00, 0x11 },
					Package (0x04) { 0x0008FFFF, 0x02, 0x00, 0x12 },
					Package (0x04) { 0x0008FFFF, 0x03, 0x00, 0x13 },
					Package (0x04) { 0x0009FFFF, 0x00, 0x00, 0x11 },
					Package (0x04) { 0x0009FFFF, 0x01, 0x00, 0x12 },
					Package (0x04) { 0x0009FFFF, 0x02, 0x00, 0x13 },
					Package (0x04) { 0x0009FFFF, 0x03, 0x00, 0x10 },
					Package (0x04) { 0x000AFFFF, 0x00, 0x00, 0x12 }, /* FireWire */
					Package (0x04) { 0x000AFFFF, 0x01, 0x00, 0x13 },
					Package (0x04) { 0x000AFFFF, 0x02, 0x00, 0x10 },
					Package (0x04) { 0x000AFFFF, 0x03, 0x00, 0x11 },
				})
			}
		}

		Device (ISA) {
			Name (_ADR, 0x000010000)

			/* PS/2 keyboard (seems to be important for WinXP install) */
			Device (KBD)
			{
				Name (_HID, EisaId ("PNP0303"))
				Method (_STA, 0, NotSerialized)
				{
					Return (0x0f)
				}
				Method (_CRS, 0, NotSerialized)
				{
					Name (TMP0, ResourceTemplate () {
						IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
						IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
						IRQNoFlags () {1}
					})
					Return (TMP0)
				}
			}

			/* PS/2 mouse */
			Device (MOU)
			{
				Name (_HID, EisaId ("PNP0F13"))
				Method (_STA, 0, NotSerialized)
				{
					Return (0x0f)
				}
				Method (_CRS, 0, NotSerialized)
				{
					Name (TMP1, ResourceTemplate () {
						IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
						IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
						IRQNoFlags () {12}
					})
					Return (TMP1)
				}
			}

			/* PS/2 floppy controller */
			Device (FDC0)
			{
				Name (_HID, EisaId ("PNP0700"))
				Method (_STA, 0, NotSerialized)
				{
					Return (0x0f)
				}
				Method (_CRS, 0, NotSerialized)
				{
					Name (BUF0, ResourceTemplate () {
						IO (Decode16, 0x03F0, 0x03F0, 0x01, 0x06)
						IO (Decode16, 0x03F7, 0x03F7, 0x01, 0x01)
						IRQNoFlags () {6}
						DMA (Compatibility, NotBusMaster, Transfer8) {2}
					})
					Return (BUF0)
				}
			}
			/* Parallel Port */
			Device (LPT1)
			{
				Name (_HID, EisaId ("PNP0400"))
				Method (_STA, 0, NotSerialized)
				{
					Return (0x0f)
				}
				Method (_CRS, 0, NotSerialized)
				{
					Name (BUF1, ResourceTemplate () {
						IO (Decode16, 0x0378, 0x0378, 0x01, 0x08)
						IRQNoFlags () {7}
					})
					Return (BUF1)
				}
			}
			/* Parallel Port ECP */
			Device (ECP1)
			{
				Name (_HID, EisaId ("PNP0401"))
				Method (_STA, 0, NotSerialized)
				{
					Return (0x0f)
				}
				Method (_CRS, 0, NotSerialized)
				{
					Name (BUF1, ResourceTemplate () {
						IO (Decode16, 0x0378, 0x0378, 0x01, 0x04)
						IO (Decode16, 0x0778, 0x0778, 0x01, 0x04)
						IRQNoFlags() {7}
						DMA (Compatibility, NotBusMaster, Transfer8) {0,1,3}
					})
					Return (BUF1)
				}
			}
		}
	}
}
