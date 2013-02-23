/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <Nick.Barker9@btinternet.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

DefinitionBlock ("DSDT.aml", "DSDT", 1, "CORE  ", "COREBOOT", 1)
{
	 #include "northbridge/amd/amdk8/util.asl"

	/* For now only define 2 power states:
	 *  - S0 which is fully on
	 *  - S5 which is soft off
	 * Any others would involve declaring the wake up methods.
	 */
	Name (\_S0, Package () { 0x00, 0x00, 0x00, 0x00 })
	Name (\_S5, Package () { 0x02, 0x02, 0x00, 0x00 })

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
				Package (0x04) { 0x000BFFFF, 0x00, 0x00, 0x10 }, /* Slot 0xB */
				Package (0x04) { 0x000BFFFF, 0x01, 0x00, 0x11 },
				Package (0x04) { 0x000BFFFF, 0x02, 0x00, 0x12 },
				Package (0x04) { 0x000BFFFF, 0x03, 0x00, 0x13 },
				Package (0x04) { 0x000CFFFF, 0x00, 0x00, 0x11 }, /* Slot 0xC */
				Package (0x04) { 0x000CFFFF, 0x01, 0x00, 0x12 },
				Package (0x04) { 0x000CFFFF, 0x02, 0x00, 0x13 },
				Package (0x04) { 0x000CFFFF, 0x03, 0x00, 0x10 },
				Package (0x04) { 0x000DFFFF, 0x00, 0x00, 0x12 }, /* Slot 0xD */
				Package (0x04) { 0x000DFFFF, 0x01, 0x00, 0x13 },
				Package (0x04) { 0x000DFFFF, 0x02, 0x00, 0x10 },
				Package (0x04) { 0x000DFFFF, 0x03, 0x00, 0x11 },
				Package (0x04) { 0x000FFFFF, 0x01, 0x00, 0x14 }, /* 0xf SATA IRQ 20 */
				Package (0x04) { 0x000FFFFF, 0x00, 0x00, 0x14 }, /* 0xf Native IDE IRQ 20 */
				Package (0x04) { 0x0010FFFF, 0x00, 0x00, 0x15 }, /* USB routing */
				Package (0x04) { 0x0010FFFF, 0x01, 0x00, 0x15 },
				Package (0x04) { 0x0010FFFF, 0x02, 0x00, 0x15 },
				Package (0x04) { 0x0010FFFF, 0x03, 0x00, 0x15 },
				Package (0x04) { 0x0011FFFF, 0x02, 0x00, 0x16 }, /* AC97, MC97 */
				Package (0x04) { 0x0002FFFF, 0x00, 0x00, 0x1B }, /* PCIE16 bridge IRQ27 */
				Package (0x04) { 0x0002FFFF, 0x01, 0x00, 0x1B },
				Package (0x04) { 0x0002FFFF, 0x02, 0x00, 0x1B },
				Package (0x04) { 0x0002FFFF, 0x03, 0x00, 0x1B },
				Package (0x04) { 0x0003FFFF, 0x00, 0x00, 0x1F }, /* PCIE bridge IRQ31 */
				Package (0x04) { 0x0003FFFF, 0x01, 0x00, 0x23 }, /* IRQ36 */
				Package (0x04) { 0x0003FFFF, 0x02, 0x00, 0x27 }, /* IRQ39 */
				Package (0x04) { 0x0003FFFF, 0x03, 0x00, 0x2B }  /* IRQ43 */
			})

			Device (PEGG)
			{
				Name (_ADR, 0x00020000)
				Name (_UID, 0x00)
				Name (_BBN, 0x02)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x18 }, /* PCIE IRQ24-IRQ27 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x19 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x1A },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x1B },
				})
			}

			Device (PEX0)
			{
				Name (_ADR, 0x00030000)
				Name (_UID, 0x00)
				Name (_BBN, 0x03)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x1C }, /* PCIE IRQ28-IRQ31 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x1D },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x1E },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x1F },
				})
			}

			Device (PEX1)
			{
				Name (_ADR, 0x00030001)
				Name (_UID, 0x00)
				Name (_BBN, 0x04)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x20 }, /* PCIE IRQ32-IRQ35 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x21 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x22 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x23 },
				})
			}

			Device (PEX2)
			{
				Name (_ADR, 0x00030002)
				Name (_UID, 0x00)
				Name (_BBN, 0x05)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x24 }, /* PCIE IRQ36-IRQ39 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x25 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x26 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x27 },
				})
			}

			Device (PEX3)
			{
				Name (_ADR, 0x00030003)
				Name (_UID, 0x00)
				Name (_BBN, 0x06)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x28 }, /* PCIE IRQ40-IRQ43 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x29 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x2A },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x2B },
				})
			}

			Device (ISA) {
				Name (_ADR, 0x00110000)

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
							IO (Decode16, 0x03F2, 0x03F2, 0x00, 0x04)
							IO (Decode16, 0x03F7, 0x03F7, 0x00, 0x01)
							IRQNoFlags () {6}
							DMA (Compatibility, NotBusMaster, Transfer8) {2}
						})
						Return (BUF0)
					}
				}
			}
			/* Dummy device to hold auto generated reserved resources */
			Device(MBRS) {
				Name (_HID, EisaId ("PNP0C02"))
				Name (_UID, 0x01)
				External(_CRS) /* Resource Template in SSDT */
			}

		}
	}
}
