/*
 * This file is part of the coreboot project.
 *
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * ISA portions taken from QEMU acpi-dsdt.dsl.
 */

DefinitionBlock ("DSDT.aml", "DSDT", 1, "CORE  ", "CB-DSDT ", 1)
{
	#include "northbridge/amd/amdk8/util.asl"

	/* For now only define 2 power states:
	 *  - S0 which is fully on
	 *  - S5 which is soft off
	 * Any others would involve declaring the wake up methods.
	 */
	Name (\_S0, Package () { 0x00, 0x00, 0x00, 0x00 })
	Name (\_S5, Package () { 0x07, 0x00, 0x00, 0x00 })

	Name (PICM, 0x00)
	Method (_PIC, 1, Serialized) {
		Store (Arg0, PICM)
	}

	/* Root of the bus hierarchy */
	Scope (\_SB)
	{
		/* Top PCI device (CK804) */
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
				/* Methods bellow use SSDT to get actual MMIO regs
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

			/* PCI Routing Table */
			Name (_PRT, Package () {
				Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI0.LLAS, 0x00 },
				Package (0x04) { 0x0001FFFF, 0x01, \_SB.PCI0.LLAS, 0x00 },
				Package (0x04) { 0x0002FFFF, 0x00, \_SB.PCI0.LUOH, 0x00 },
				Package (0x04) { 0x0002FFFF, 0x01, \_SB.PCI0.LUEH, 0x00 },
				Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI0.LAUD, 0x00 },
				Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI0.LMOD, 0x00 },
				Package (0x04) { 0x0006FFFF, 0x00, \_SB.PCI0.LPA0, 0x00 },
				Package (0x04) { 0x0007FFFF, 0x00, \_SB.PCI0.LSA0, 0x00 },
				Package (0x04) { 0x0008FFFF, 0x00, \_SB.PCI0.LSA1, 0x00 },

				Package (0x04) { 0x0009FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },
				Package (0x04) { 0x0009FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
				Package (0x04) { 0x0009FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
				Package (0x04) { 0x0009FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },

				Package (0x04) { 0x000AFFFF, 0x00, \_SB.PCI0.LEMA, 0x00 },

				Package (0x04) { 0x000BFFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
				Package (0x04) { 0x000BFFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },

				Package (0x04) { 0x000CFFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
				Package (0x04) { 0x000CFFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },

				Package (0x04) { 0x000DFFFF, 0x00, \_SB.PCI0.LNKD, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x01, \_SB.PCI0.LNKA, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x02, \_SB.PCI0.LNKB, 0x00 },
				Package (0x04) { 0x000DFFFF, 0x03, \_SB.PCI0.LNKC, 0x00 },

				Package (0x04) { 0x000EFFFF, 0x00, \_SB.PCI0.LNKC, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x01, \_SB.PCI0.LNKD, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x02, \_SB.PCI0.LNKA, 0x00 },
				Package (0x04) { 0x000EFFFF, 0x03, \_SB.PCI0.LNKB, 0x00 },
			})

			Device (PCIC)
			{
				Name (_ADR, 0x00090000)
				Name (_UID, 0x00)
				Name (_PRT, Package () {
					/* AGR slot */
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x10 },
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x10 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x10 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x10 },
				})
			}

			/* 2:00 PCIe x1 */
			Device (PEX1)
			{
				Name (_ADR, 0x000d0000)
				Name (_UID, 0x00)
			}

			/* 3:00 PCIe x16 */
			Device (PEX0)
			{
				Name (_ADR, 0x000e0000)
				Name (_UID, 0x00)
			}

			Device (LPC) {
				Name (_HID, EisaId ("PNP0A05"))
				Name (_ADR, 0x00010000)

				OperationRegion (CF44, PCI_Config, 0x44, 0x04)
				Field (CF44, ByteAcc, NoLock, Preserve)
				{
					ETBA, 32,
				}

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
					Name (_HID, EisaId ("PNP0F13"))
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0f)
					}
					Method (_CRS, 0, NotSerialized)
					{
						Name (TMP, ResourceTemplate () {
							IRQNoFlags () {12}
						})
						Return (TMP)
					}
				}

				/* Parallel port */
				Device (LP0)
				{
					Name (_HID, EisaId ("PNP0400")) // "PNP0401" for ECP
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0f)
					}
					Method (_CRS, 0, NotSerialized)
					{
						Name (TMP, ResourceTemplate () {
							FixedIO (0x0378, 0x10)
							IRQNoFlags () {7}
						})
						Return (TMP)
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
					Method (_CRS, 0, NotSerialized)
					{
						Name (BUF0, ResourceTemplate () {
							FixedIO (0x03F0, 0x08)
							IRQNoFlags () {6}
							DMA (Compatibility, NotBusMaster, Transfer8) {2}
						})
						Return (BUF0)
					}
				}
#if 0
				Device (HPET)
				{
					Name (_HID, EisaId ("PNP0103"))
					Name (CRS, ResourceTemplate ()
					{
						Memory32Fixed (ReadOnly,
						0x00000000,
						0x00001000,
						_Y02)
					})
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0F)
					}
					Method (_CRS, 0, NotSerialized)
					{
						CreateDWordField (CRS, \_SB.PCI0.LPC.HPET._Y02._BAS, HPT)
						Store (ETBA, HPT)
						Return (CRS)
					}

				}
#endif
			}
		}
	}
}
