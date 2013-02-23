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
				Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x10 }, /* AGP slot, effectively */
				Package (0x04) { 0x0001FFFF, 0x01, 0x00, 0x11 },
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
				Package (0x04) { 0x000EFFFF, 0x00, 0x00, 0x13 }, /* Slot 0xE */
				Package (0x04) { 0x000EFFFF, 0x01, 0x00, 0x10 },
				Package (0x04) { 0x000EFFFF, 0x02, 0x00, 0x11 },
				Package (0x04) { 0x000EFFFF, 0x03, 0x00, 0x12 },
				Package (0x04) { 0x0009FFFF, 0x00, 0x00, 0x10 }, /* Slot 0x9 */
				Package (0x04) { 0x0009FFFF, 0x01, 0x00, 0x11 },
				Package (0x04) { 0x0009FFFF, 0x02, 0x00, 0x12 },
				Package (0x04) { 0x0009FFFF, 0x03, 0x00, 0x13 },
				Package (0x04) { 0x000AFFFF, 0x00, 0x00, 0x11 }, /* Marvell 88E8001 ethernet */
				Package (0x04) { 0x000FFFFF, 0x01, 0x00, 0x14 }, /* 0xf SATA IRQ 20 */
				Package (0x04) { 0x000FFFFF, 0x00, 0x00, 0x14 }, /* 0xf Native IDE IRQ 20 */
				Package (0x04) { 0x0010FFFF, 0x00, 0x00, 0x15 }, /* USB routing */
				Package (0x04) { 0x0010FFFF, 0x01, 0x00, 0x15 },
				Package (0x04) { 0x0010FFFF, 0x02, 0x00, 0x15 },
				Package (0x04) { 0x0011FFFF, 0x02, 0x00, 0x16 }  /* AC97, MC97 */
			})

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
