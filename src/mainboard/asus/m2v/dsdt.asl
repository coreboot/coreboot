/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <Nick.Barker9@btinternet.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
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

#define LNKA INTA
#define LNKB INTB
#define LNKC INTC
#define LNKD INTD

/*
 * For simplicity map LNK[E-H] to LNK[A-D].
 * This also means we are 82C596 compatible.
 * Needs 0:11.0 0x46[4] set to 0.
 */
#define LNKE INTA
#define LNKF INTB
#define LNKG INTC
#define LNKH INTD

DefinitionBlock ("DSDT.aml", "DSDT", 2, "CORE  ", "COREBOOT", 1)
{
	Name(APIC, 0)		// 0=>8259, 1=>IOAPIC

	/* The _PIC method is called by the OS to choose between interrupt
	 * routing via the i8259 interrupt controller or the APIC.
	 *
	 * _PIC is called with a parameter of 0 for i8259 configuration and
	 * with a parameter of 1 for Local Apic/IOAPIC configuration.
	 */

	Method(_PIC, 1)
	{
		// Remember the OS' IRQ routing choice.
		Store(Arg0, APIC)
	}

	/* _PR CPU0 is dynamically supplied by SSDT */

	/* We define 3 power states:
	 *  - S0 which is fully on
	 *  - S3 which is suspend to ram
	 *  - S5 which is soft off
	 *
	 * Package contents:
	 * ofs len desc
	 * 0   1   Value for PM1a_CNT.SLP_TYP register to enter this system state.
	 * 1   1   Value for PM1b_CNT.SLP_TYP register to enter this system state. To enter any
	 *         given state, OSPM must write the PM1a_CNT.SLP_TYP register before the
	 *         PM1b_CNT.SLP_TYP register.
	 * 2   2   Reserved
	 */
	Name (\_S0, Package () { 0x00, 0x00, 0x00, 0x00 })
	Name (\_S3, Package () { 0x01, 0x01, 0x00, 0x00 })
	Name (\_S5, Package () { 0x02, 0x02, 0x00, 0x00 })

	/* Root of the bus hierarchy */
	Scope (\_SB)
	{
		/* Top PCI device */
		Device (PCI0)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00180000)
			Name (_BBN, 0x00)

			Name (APRT, Package() {
				/* AGP? */
				Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x10 },
				Package (0x04) { 0x0001FFFF, 0x01, 0x00, 0x11 },
				Package (0x04) { 0x0001FFFF, 0x02, 0x00, 0x12 },
				Package (0x04) { 0x0001FFFF, 0x03, 0x00, 0x13 },
				/* PCIe graphics bridge */
				Package (0x04) { 0x0002FFFF, 0x00, 0x00, 0x1B },
				Package (0x04) { 0x0002FFFF, 0x01, 0x00, 0x1B },
				Package (0x04) { 0x0002FFFF, 0x02, 0x00, 0x1B },
				Package (0x04) { 0x0002FFFF, 0x03, 0x00, 0x1B },
				/* PCIe bridge */
				Package (0x04) { 0x0003FFFF, 0x00, 0x00, 0x1F },
				Package (0x04) { 0x0003FFFF, 0x01, 0x00, 0x23 },
				Package (0x04) { 0x0003FFFF, 0x02, 0x00, 0x27 },
				Package (0x04) { 0x0003FFFF, 0x03, 0x00, 0x2B },
				/* SATA */
				Package (0x04) { 0x000FFFFF, 0x01, 0x00, 0x15 },
				/* IDE */
				Package (0x04) { 0x000FFFFF, 0x00, 0x00, 0x15 },
				/* USB */
				Package (0x04) { 0x0010FFFF, 0x00, 0x00, 0x14 },
				Package (0x04) { 0x0010FFFF, 0x01, 0x00, 0x16 },
				Package (0x04) { 0x0010FFFF, 0x02, 0x00, 0x15 },
				Package (0x04) { 0x0010FFFF, 0x03, 0x00, 0x17 },
				/* PCI bridge */
				Package (0x04) { 0x0013FFFF, 0x00, 0x00, 0x14 },
				Package (0x04) { 0x0013FFFF, 0x01, 0x00, 0x14 },
				Package (0x04) { 0x0013FFFF, 0x02, 0x00, 0x14 },
				Package (0x04) { 0x0013FFFF, 0x03, 0x00, 0x14 },
			})
			Name (PPRT, Package() {
				/* ?? */
				Package (0x04) { 0x0001FFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0x0001FFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0x0001FFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0x0001FFFF, 0x03, LNKD, 0x00 },
				/* PCIe graphics bridge */
				Package (0x04) { 0x0002FFFF, 0x00, LNKH, 0x00 },
				Package (0x04) { 0x0002FFFF, 0x01, LNKH, 0x00 },
				Package (0x04) { 0x0002FFFF, 0x02, LNKH, 0x00 },
				Package (0x04) { 0x0002FFFF, 0x03, LNKH, 0x00 },
				/* PCIe bridge */
				Package (0x04) { 0x0003FFFF, 0x00, LNKH, 0x00 },
				Package (0x04) { 0x0003FFFF, 0x01, LNKH, 0x00 },
				Package (0x04) { 0x0003FFFF, 0x02, LNKH, 0x00 },
				Package (0x04) { 0x0003FFFF, 0x03, LNKH, 0x00 },
				/* SATA */
				Package (0x04) { 0x000FFFFF, 0x01, LNKB, 0x00 },
				/* USB */
				Package (0x04) { 0x0010FFFF, 0x00, LNKA, 0x00 },
				Package (0x04) { 0x0010FFFF, 0x01, LNKB, 0x00 },
				Package (0x04) { 0x0010FFFF, 0x02, LNKC, 0x00 },
				Package (0x04) { 0x0010FFFF, 0x03, LNKD, 0x00 },
				/* PCI bridge */
				Package (0x04) { 0x0013FFFF, 0x00, LNKD, 0x00 },
				Package (0x04) { 0x0013FFFF, 0x01, LNKD, 0x00 },
				Package (0x04) { 0x0013FFFF, 0x02, LNKD, 0x00 },
				Package (0x04) { 0x0013FFFF, 0x03, LNKD, 0x00 },
			})

			/* PCI Routing Table */
			Method (_PRT, 0, NotSerialized)
			{
				If (APIC)
				{
					Return (APRT)
				}
				Return (PPRT)
			}

			Device (PEGG)
			{
				Name (_ADR, 0x00020000)
				Name (APRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x18 }, /* PCIE IRQ24-IRQ27 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x19 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x1A },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x1B },
				})
				Name (PPRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x01, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x02, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x03, LNKH, 0x00 },
				})
				Method (_PRT, 0, NotSerialized)
				{
					If (APIC)
					{
						Return (APRT)
					}
					Return (PPRT)
				}
			}

			Device (PEX0)
			{
				Name (_ADR, 0x00030000)
				Name (APRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x1C }, /* PCIE IRQ28-IRQ31 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x1D },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x1E },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x1F },
				})
				Name (PPRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x01, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x02, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x03, LNKH, 0x00 },
				})
				Method (_PRT, 0, NotSerialized)
				{
					If (APIC)
					{
						Return (APRT)
					}
					Return (PPRT)
				}
			}

			Device (PEX1)
			{
				Name (_ADR, 0x00030001)
				Name (APRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x20 }, /* PCIE IRQ32-IRQ35 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x21 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x22 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x23 },
				})
				Name (PPRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x01, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x02, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x03, LNKH, 0x00 },
				})
				Method (_PRT, 0, NotSerialized)
				{
					If (APIC)
					{
						Return (APRT)
					}
					Return (PPRT)
				}
			}

			Device (PEX2)
			{
				Name (_ADR, 0x00030002)
				Name (APRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x24 }, /* PCIE IRQ36-IRQ39 */
					Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x25 },
					Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x26 },
					Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x27 },
				})
				Name (PPRT, Package () {
					Package (0x04) { 0x0000FFFF, 0x00, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x01, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x02, LNKH, 0x00 },
					Package (0x04) { 0x0000FFFF, 0x03, LNKH, 0x00 },
				})
				Method (_PRT, 0, NotSerialized)
				{
					If (APIC)
					{
						Return (APRT)
					}
					Return (PPRT)
				}
			}

			Device (PCI6)
			{
				Name (_ADR, 0x00130000)
				Name (APRT, Package () {
					Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x11 }, /* IRQ17 */
				})
				Name (PPRT, Package () {
					Package (0x04) { 0x0001FFFF, 0x00, LNKB, 0x00 },
				})
				Method (_PRT, 0, NotSerialized)
				{
					If (APIC)
					{
						Return (APRT)
					}
					Return (PPRT)
				}
			}

			Device (PCI7)
			{
				Name (_ADR, 0x00130001)
				Name (APRT, Package () {
					/* PCI slot 1 */
					Package (0x04) { 0x0006FFFF, 0x00, 0x00, 0x10 },
					Package (0x04) { 0x0006FFFF, 0x01, 0x00, 0x11 },
					Package (0x04) { 0x0006FFFF, 0x02, 0x00, 0x12 },
					Package (0x04) { 0x0006FFFF, 0x03, 0x00, 0x13 },

					/* PCI slot 2 */
					Package (0x04) { 0x0007FFFF, 0x00, 0x00, 0x11 },
					Package (0x04) { 0x0007FFFF, 0x01, 0x00, 0x12 },
					Package (0x04) { 0x0007FFFF, 0x02, 0x00, 0x13 },
					Package (0x04) { 0x0007FFFF, 0x03, 0x00, 0x10 },

					/* PCI slot 3 */
					Package (0x04) { 0x0008FFFF, 0x00, 0x00, 0x12 },
					Package (0x04) { 0x0008FFFF, 0x01, 0x00, 0x13 },
					Package (0x04) { 0x0008FFFF, 0x02, 0x00, 0x10 },
					Package (0x04) { 0x0008FFFF, 0x03, 0x00, 0x11 },

					/* PCI slot 4 */
					Package (0x04) { 0x0009FFFF, 0x00, 0x00, 0x13 },
					Package (0x04) { 0x0009FFFF, 0x01, 0x00, 0x10 },
					Package (0x04) { 0x0009FFFF, 0x02, 0x00, 0x11 },
					Package (0x04) { 0x0009FFFF, 0x03, 0x00, 0x12 },
				})
				Name (PPRT, Package () {
					/* PCI slot 1 */
					Package (0x04) { 0x0006FFFF, 0x00, LNKA, 0x00 },
					Package (0x04) { 0x0006FFFF, 0x01, LNKB, 0x00 },
					Package (0x04) { 0x0006FFFF, 0x02, LNKC, 0x00 },
					Package (0x04) { 0x0006FFFF, 0x03, LNKD, 0x00 },

					/* PCI slot 2 */
					Package (0x04) { 0x0007FFFF, 0x00, LNKB, 0x00 },
					Package (0x04) { 0x0007FFFF, 0x01, LNKC, 0x00 },
					Package (0x04) { 0x0007FFFF, 0x02, LNKD, 0x00 },
					Package (0x04) { 0x0007FFFF, 0x03, LNKA, 0x00 },

					/* PCI slot 3 */
					Package (0x04) { 0x0008FFFF, 0x00, LNKC, 0x00 },
					Package (0x04) { 0x0008FFFF, 0x01, LNKD, 0x00 },
					Package (0x04) { 0x0008FFFF, 0x02, LNKA, 0x00 },
					Package (0x04) { 0x0008FFFF, 0x03, LNKB, 0x00 },

					/* PCI slot 4 */
					Package (0x04) { 0x0009FFFF, 0x00, LNKD, 0x00 },
					Package (0x04) { 0x0009FFFF, 0x01, LNKA, 0x00 },
					Package (0x04) { 0x0009FFFF, 0x02, LNKB, 0x00 },
					Package (0x04) { 0x0009FFFF, 0x03, LNKC, 0x00 },
				})

				Method (_PRT, 0, NotSerialized)
				{
					If (APIC)
					{
						Return (APRT)
					}
					Return (PPRT)
				}
			}

			Device (SBRG) { /* southbridge */
				Name (_ADR, 0x00110000)
				OperationRegion (PCIC, PCI_Config, 0x0, 0x100)

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
				Device (LPT0)
				{
					Name (_HID, EisaId ("PNP0401"))
					Method (_STA, 0, NotSerialized)
					{
						Return (0x0f)
					}
					Method (_CRS, 0, NotSerialized)
					{
						Name (TMP, ResourceTemplate () {
							IO (Decode16, 0x0378, 0x0378, 0x01, 0x08)
							IO (Decode16, 0x0778, 0x0778, 0x01, 0x08)
							IRQNoFlags () {7}
							DMA (Compatibility, NotBusMaster, Transfer8) {3}
						})
						Return (TMP)
					}
				}
			}

			Device(MBRS) {
				Name (_HID, EisaId ("PNP0C02"))
				Name (_UID, 0x01)

				External(_CRS) /* Resource Template in SSDT */
			}

			External(TOM1) /* top of memory below 4GB */

			Method(_CRS, 0) {
				Name(TMP, ResourceTemplate() {
					WordBusNumber(ResourceProducer, MinFixed, MaxFixed, PosDecode,
						0x0000,             // Granularity
						0x0000,             // Range Minimum
						0x00FF,             // Range Maximum
						0x0000,             // Translation Offset
						0x0100,             // Length
						,,
					)
					IO(Decode16, 0x0CF8, 0x0CF8, 1, 8)

					WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
						0x0000,			/* address granularity */
						0x0000,			/* range minimum */
						0x0CF7,			/* range maximum */
						0x0000,			/* translation */
						0x0CF8			/* length */
					)

					WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
						0x0000,			/* address granularity */
						0x0D00,			/* range minimum */
						0xFFFF,			/* range maximum */
						0x0000,			/* translation */
						0xF300			/* length */
					)

					/* memory space for PCI BARs below 4GB */
					Memory32Fixed(ReadOnly, 0x00000000, 0x00000000, MMIO)
				})
				CreateDWordField(TMP, MMIO._BAS, MM1B)
				CreateDWordField(TMP, MMIO._LEN, MM1L)
				/*
				 * Declare memory between TOM1 and 4GB as available
				 * for PCI MMIO.
				 *
				 * Use ShiftLeft to avoid 64bit constant (for XP).
				 * This will work even if the OS does 32bit arithmetic, as
				 * 32bit (0x00000000 - TOM1) will wrap and give the same
				 * result as 64bit (0x100000000 - TOM1).
				 */
				Store(TOM1, MM1B)
				ShiftLeft(0x10000000, 4, Local0)
				Subtract(Local0, TOM1, Local0)
				Store(Local0, MM1L)

				Return(TMP)
			}
		}

		Field (PCI0.SBRG.PCIC, ByteAcc, NoLock, Preserve)
		{
			Offset (0x55),
			/*
			 * Offset 0x55:
			 *    3-0: reserved
			 *    7-4: PCI INTA# routing
			 * Offset 0x56:
			 *    3-0: PCI INTB# routing
			 *    7-4: PCI INTC# routing
			 * Offset 0x57:
			 *    3-0: reserved
			 *    7-4: PCI INTD# routing
			 *
			 * Valid values for routing link:
			 * 0:                disabled
			 * 2,8,13:           reserved
			 * 1,3-7,9-12,14,15: corresponding irq
			 */
			    , 4,
			PINA, 4,
			PINB, 4,
			PINC, 4,
			    , 4,
			PIND, 4,
		}

		Name(IRQB, ResourceTemplate(){
			IRQ(Level,ActiveLow,Shared){15}
		})

		Name(IRQP, ResourceTemplate(){
			IRQ(Level,ActiveLow,Exclusive){3, 4, 5, 6, 7, 10, 11, 12}
		})

		/* adapted from ma78gm/dsdt.asl */
#define PCI_INTX_DEV(intx, pinx, uid)					\
		Device(intx) {						\
			Name(_HID, EISAID("PNP0C0F"))			\
			Name(_UID, uid)					\
									\
			Method(_STA, 0) {				\
				if (pinx) {				\
					Return(0x0B)			\
				}					\
				Return(0x09)				\
			}						\
									\
			Method(_DIS ,0) {				\
				Store(0, pinx)				\
			}						\
									\
			Method(_PRS ,0) {				\
				Return(IRQP)				\
			}						\
									\
			Method(_CRS ,0) {				\
				CreateWordField(IRQB, 1, IRQN)		\
				ShiftLeft(1, pinx, IRQN)		\
				Return(IRQB)				\
			}						\
									\
			Method(_SRS, 1) {				\
				CreateWordField(ARG0, 1, IRQM)		\
									\
				/* Use lowest available IRQ */		\
				FindSetRightBit(IRQM, Local0)		\
				if (Local0) {				\
					Decrement(Local0)		\
				}					\
				Store(Local0, pinx)			\
			}						\
		}							\

PCI_INTX_DEV(INTA, PINA, 1)
PCI_INTX_DEV(INTB, PINB, 2)
PCI_INTX_DEV(INTC, PINC, 3)
PCI_INTX_DEV(INTD, PIND, 4)
	}

	Field (_SB.PCI0.SBRG.PCIC, ByteAcc, NoLock, Preserve)
	{
		Offset (0x94),
		/* two LSB bits are blink rate */
		LEDR,   2,
	}

	Method (_PTS, 1, NotSerialized)
	{
		/* blink power led while suspended */
		Store (0x1, LEDR)
	}

	Method (_WAK, 1, NotSerialized)
	{
		/* stop power led blinking */
		Store (0x0, LEDR)
		/* wake OK */
		Return(Package(0x02){0x00, 0x00})
	}
}
