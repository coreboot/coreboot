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
	Name (\_S5, Package () { 0x02, 0x02, 0x00, 0x00 })

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

			/* PCI Routing Table */
			Name (_PRT, Package () {
						/* Since source is 0, index is IRQ. */
						/* in ABCD, A=0, B=1, C=2, D=3 */
						/* SlotFFFF, ABCD, source, index */
				Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x0A }, /* 0x1 SMBUS IRQ 10 */
				Package (0x04) { 0x0002FFFF, 0x00, 0x00, 0x15 }, /* 0x2 USB IRQ 21 */
				Package (0x04) { 0x0002FFFF, 0x01, 0x00, 0x14 }, /* 0x2 USB IRQ 20 */
				Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x14 }, /* 0x2 AUDIO IRQ 20 */
				Package (0x04) { 0x0007FFFF, 0x00, 0x00, 0x17 }, /* 0x7 SATA 0 IRQ 23 */
				Package (0x04) { 0x0008FFFF, 0x00, 0x00, 0x16 }, /* 0x8 SATA 1 IRQ 22 */
				Package (0x04) { 0x000aFFFF, 0x00, 0x00, 0x15 }, /* 0xa LAN IRQ 21 */
			})

			Device (PCIL)
			{
				Name (_ADR, 0x00090000)
				Name (_UID, 0x00)
				Name (_BBN, 0x01)
				Name (_PRT, Package () {
					Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x10 }, /* 1:04 PCI 32 IRQ16-IRQ19 */
					Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x11 },
					Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x12 },
					Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x13 },
					Package (0x04) { 0x0005FFFF, 0x00, 0x00, 0x13 }, /* 1:05 IEEE-1394 IRQ 19 */
				})
			}

			/* 2:00 PCIe x16 SB IRQ 18 */
			Device (PE16)
			{
				Name (_ADR, 0x000e0000)
				Name (_UID, 0x00)
				Name (_BBN, 0x02)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x12 }, /* PCIE IRQ16-IRQ19 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x13 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x10 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x11 },
				})
			}

			Device (ISA) {
				Name (_HID, EisaId ("PNP0A05"))
				Name (_ADR, 0x00010000)

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

				/* RTC */
				Device (RTC)
				{
					Name (_HID, EisaId ("PNP0B00"))
					Method (_CRS, 0, NotSerialized)
					{
						Name (TMP, ResourceTemplate () {
							FixedIO (0x0070, 0x02)
							IRQNoFlags () {8}
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
			}
		}

		/* CK804 2050 */
		Device (PCI1)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00)
			Name (_UID, 0x00)
			Name (_BBN, 0x80)

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
				})
				/* Methods bellow use SSDT to get actual MMIO regs
				   The IO ports are from 0xd00, optionally an VGA,
				   otherwise the info from MMIO is used.
				   \_SB.GXXX(node, link)
				 */
				Concatenate (\_SB.GMEM (0x01, 0x00), BUF0, Local1)
				Concatenate (\_SB.GIOR (0x01, 0x00), Local1, Local2)
				Concatenate (\_SB.GWBN (0x01, 0x00), Local2, Local3)
				Return (Local3)
			}

			/* PCI Routing Table for this root bus */
			Name (_PRT, Package () {
						/* Since source is 0, index is IRQ. */
						/* in ABCD, A=0, B=1, C=2, D=3 */
						/* SlotFFFF, ABCD, source, index */
				Package (0x04) { 0x000aFFFF, 0x00, 0x00, 0x35 }, /* 0xa LAN IRQ 53 */
			})

			/* PCIe x16 SB2 IRQ 18 */
			Device (PE16)
			{
				Name (_ADR, 0x000e0000)
				Name (_UID, 0x00)
				Name (_BBN, 0x81)
				Name (_PRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x32 }, /* PCIE IRQ48-IRQ51 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x33 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x30 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x31 },
				})
			}

		}

		/* AMD 8131 PCI-X tunnel */
		Device (PCI2)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00)
			Name (_UID, 0x00)
			Name (_BBN, 0x40)

			/* There is no _PRT Here because I don't know what to
			 * put in it.  Since the 8131 has its own APIC, it
			 * isn't wired to other IRQs. */

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
				})
				/* Methods bellow use SSDT to get actual MMIO regs
				   The IO ports are from 0xd00, optionally an VGA,
				   otherwise the info from MMIO is used.
				   \_SB.GXXX(node, link)
				 */
				Concatenate (\_SB.GMEM (0x00, 0x02), BUF0, Local1)
				Concatenate (\_SB.GIOR (0x00, 0x02), Local1, Local2)
				Concatenate (\_SB.GWBN (0x00, 0x02), Local2, Local3)
				Return (Local3)
			}

			/* Channel A PCIX 133 */
			Device (PCXF)
			{
				Name (_ADR, 0x00000000)
				Name (_UID, 0x00)
				Name (_BBN, 0x41)
				Name (_PRT, Package () {
					Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x18 }, /* PCIE IRQ24-IRQ27 */
					Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x19 },
					Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x1a },
					Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x1b },
				})
			}

			/* Channel B PCIX 100 */
			Device (PCXS) /* Slot 4, Onboard SCSI, Slot 5 */
			{
				Name (_ADR, 0x00010000)
				Name (_UID, 0x00)
				Name (_BBN, 0x42)
				Name (_PRT, Package () {
					Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x1c }, /* PCIE IRQ28-IRQ31 */
					Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x1d },
					Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x1e },
					Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x1f },
					Package (0x04) { 0x0006FFFF, 0x00, 0x00, 0x1e }, /* PCIE IRQ28-IRQ31 shifted 2 */
					Package (0x04) { 0x0006FFFF, 0x01, 0x00, 0x1f },
					Package (0x04) { 0x0006FFFF, 0x02, 0x00, 0x1c },
					Package (0x04) { 0x0006FFFF, 0x03, 0x00, 0x1d },
					Package (0x04) { 0x0009FFFF, 0x00, 0x00, 0x1d }, /* PCIE IRQ28-IRQ31 shifted 1 */
					Package (0x04) { 0x0009FFFF, 0x01, 0x00, 0x1e },
					Package (0x04) { 0x0009FFFF, 0x02, 0x00, 0x1f },
					Package (0x04) { 0x0009FFFF, 0x03, 0x00, 0x1c },
				})
			}
		}
	}
}
