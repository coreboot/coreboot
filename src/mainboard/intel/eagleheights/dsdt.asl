/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Thomas Jourdan <thomas.jourdan@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

DefinitionBlock ("DSDT", "DSDT", 1, "EAGLE", "COREBOOT", 0x0000001)
{
	Scope (\_PR)
	{
		Processor (CPU1, 0x01, 0x00000810, 0x06)
		{
			OperationRegion (STBL, SystemMemory, 0xFFFF0000, 0xFFFF)
			Name (NCPU, 0x80)
			Name (TYPE, 0x80000000)
			Name (HNDL, 0x80000000)
			Name (CFGD, 0x80000000)
			Name (TBLD, 0x80)
			Method (_PDC, 1, NotSerialized)
			{
			}
		}
	}

	Scope (\_PR)
	{
		Processor (CPU2, 0x02, 0x00000000, 0x00)
		{
			OperationRegion (STBL, SystemMemory, 0xFFFF0000, 0xFFFF)
			Name (NCPU, 0x80)
			Name (TYPE, 0x80000000)
			Name (HNDL, 0x80000000)
			Name (CFGD, 0x80000000)
			Name (TBLD, 0x80)
			Method (_PDC, 1, NotSerialized)
			{
			}
		}
	}

	/* For now only define 2 power states:
	*  - S0 which is fully on
	*  - S5 which is soft off
	* Any others would involve declaring the wake up methods.
	*/
	Name (\_S0, Package () { 0x00, 0x00, 0x00, 0x00 })
	Name (\_S5, Package () { 0x02, 0x02, 0x00, 0x00 })

	Name (PICM, 0x00)
	Method (_PIC, 1, NotSerialized)
	{
		Store (Arg0, PICM)
	}

	/* System bus */
	Scope (\_SB)
	{
		/* Routing PCI0 */
		Name (PR00, Package (0x0E)
		{
		Package (0x04){0x0001FFFF,0x00,LNKA,0x00}, /* EDMA INTA# */
		Package (0x04){0x0002FFFF,0x00,LNKA,0x00}, /* PCIe port A */
		Package (0x04){0x0002FFFF,0x01,LNKB,0x00},
		Package (0x04){0x0002FFFF,0x02,LNKC,0x00},
		Package (0x04){0x0002FFFF,0x03,LNKD,0x00},
		Package (0x04){0x0003FFFF,0x00,LNKA,0x00}, /* PCIe port A1 */
		Package (0x04){0x0003FFFF,0x01,LNKB,0x00},
		Package (0x04){0x0003FFFF,0x02,LNKC,0x00},
		Package (0x04){0x0003FFFF,0x03,LNKD,0x00},
		Package (0x04){0x001CFFFF,0x00,LNKE,0x00}, /* PCIe port B */
			Package (0x04){0x001DFFFF,0x00,LNKH,0x00}, /* UHCI/EHCI INTA# */
			Package (0x04){0x001DFFFF,0x01,LNKD,0x00}, /* UHCI INTB# */
		Package (0x04){0x001FFFFF,0x01,LNKD,0x00}, /* SATA/SMBUS INTB# */
			Package (0x04){0x001FFFFF,0x03,LNKA,0x00}  /* CHAP INTD# */
		})
		Name (AR00, Package (0x0E)
		{
		Package (0x04){0x0001FFFF,0x00,0x00,0x10}, /* EDMA INTA# */
		Package (0x04){0x0002FFFF,0x00,0x00,0x10}, /* PCIe port A0 */
		Package (0x04){0x0002FFFF,0x01,0x00,0x11},
		Package (0x04){0x0002FFFF,0x02,0x00,0x12},
		Package (0x04){0x0002FFFF,0x03,0x00,0x13},
		Package (0x04){0x0003FFFF,0x00,0x00,0x10}, /* PCIe port A1 */
		Package (0x04){0x0003FFFF,0x01,0x00,0x11},
		Package (0x04){0x0003FFFF,0x02,0x00,0x12},
		Package (0x04){0x0003FFFF,0x03,0x00,0x13},
		Package (0x04){0x001CFFFF,0x00,0x00,0x14}, /* PCIe port B */
			Package (0x04){0x001DFFFF,0x00,0x00,0x17}, /* UHCI/EHCI INTA# */
			Package (0x04){0x001DFFFF,0x01,0x00,0x13}, /* UHCI INTB# */
		Package (0x04){0x001FFFFF,0x01,0x00,0x13}, /* SATA/SMBUS INTB# */
			Package (0x04){0x001FFFFF,0x0D,0x00,0x10}  /* CHAP INTD# */
		})
	/* Routing PCIe Port A */
		Name (PR0A, Package (0x04)
		{
			Package (0x04){0xFFFF,0x00,LNKA,0x00},
			Package (0x04){0xFFFF,0x01,LNKB,0x00},
			Package (0x04){0xFFFF,0x02,LNKC,0x00},
			Package (0x04){0xFFFF,0x03,LNKD,0x00}
		})
		Name (AR0A, Package (0x04)
		{
			Package (0x04){0xFFFF,0x00,0x00,0x10},
			Package (0x04){0xFFFF,0x01,0x00,0x11},
			Package (0x04){0xFFFF,0x02,0x00,0x12},
			Package (0x04){0xFFFF,0x03,0x00,0x13}
		})
	/* Routing PCIe Port B */
		Name (PR0B, Package (0x04)
		{
			Package (0x04){0xFFFF,0x00,LNKA,0x00},
			Package (0x04){0xFFFF,0x01,LNKB,0x00},
			Package (0x04){0xFFFF,0x02,LNKC,0x00},
			Package (0x04){0xFFFF,0x03,LNKD,0x00}
		})
		Name (AR0B, Package (0x04)
		{
			Package (0x04){0xFFFF,0x00,0x00,0x10},
			Package (0x04){0xFFFF,0x01,0x00,0x11},
			Package (0x04){0xFFFF,0x02,0x00,0x12},
			Package (0x04){0xFFFF,0x03,0x00,0x13}
		})
	/* Routing Bus PCI */
	Name (PR01, Package (0x04)
		{
		Package (0x04){0x0000FFFF,0x00,LNKA,0x00},
		Package (0x04){0x0000FFFF,0x01,LNKB,0x00},
		Package (0x04){0x0000FFFF,0x02,LNKC,0x00},
		Package (0x04){0x0000FFFF,0x03,LNKD,0x00},
	})
	Name (AR01, Package (0x04)
		{
		Package (0x04){0x0000FFFF,0x00,0x00,0x10},
		Package (0x04){0x0000FFFF,0x01,0x00,0x11},
		Package (0x04){0x0000FFFF,0x02,0x00,0x12},
		Package (0x04){0x0000FFFF,0x03,0x00,0x13},
	})

		Name (PRSA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared, )
				{3,4,5,6,7,10,11,12,14,15}
		})
		Alias (PRSA, PRSB)
		Alias (PRSA, PRSC)
		Alias (PRSA, PRSD)
		Alias (PRSA, PRSE)
		Alias (PRSA, PRSF)
		Alias (PRSA, PRSG)
		Alias (PRSA, PRSH)

	Device (PCI0)
	{
		Name (_HID, EisaId ("PNP0A08"))
		Name (_CID, EisaId ("PNP0A03"))
		Name (_ADR, 0x00)
		Name (_SEG, 0x00)
		Name (_UID, 0x00)
		Name (_BBN, 0x00)

		Name (SUPP, 0) /* PCI _OSC Support Field Value */
		Name (CTRL, 0) /* PCI _OSC Control Field Value */

		Method (_OSC, 4)
		{
			/* Check for proper GUID */
			If (LEqual (Arg0, ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")))
			{
				/* Let OS control everything */
				Return (Arg3)
			}
		} /* End _OSC */

			Method (_PRT, 0, NotSerialized)
			{
				If (PICM)
				{
					Return (AR00)
				}

				Return (PR00)
			}

		/* PCI Express Port A */
		Device (EPA0)
			{
				Name (_ADR, 0x00020000)
				Method (_PRT, 0, NotSerialized)
				{
					If (PICM)
					{
						Return (AR0A)
					}

					Return (PR0A)
				}
			}

		/* PCI Express Port A1 */
			Device (EPA1)
			{
				Name (_ADR, 0x00030000)
				Method (_PRT, 0, NotSerialized)
				{
					If (PICM)
					{
						Return (AR0A)
					}

					Return (PR0A)
				}
			}

		/* PCI Express Port B0 */
			Device (EPB0)
			{
				Name (_ADR, 0x001C0000)
				Method (_PRT, 0, NotSerialized)
				{
					If (PICM)
					{
						Return (AR0B)
					}

					Return (PR0B)
				}
			}

		/* PCI Bridge */
			Device (P0P1)
			{
				Name (_ADR, 0x001E0000)

				Method (_PRT, 0, NotSerialized)
				{
					If (PICM)
					{
						Return (AR01)
					}

					Return (PR01)
				}
			}

		/* LPC I/F Bridge */
		Device (ISA) {
			Name (_ADR, 0x001F0000)

				/* MMCONF */
				Device (^PCIE)
				{
					Name (_HID, EisaId ("PNP0C02"))
					Name (_UID, 0x11)
					Name (CRS, ResourceTemplate ()
					{
						Memory32Fixed (ReadOnly,
							0xE0000000,         // Address Base
							0x10000000,         // Address Length
							_Y10)
					})
					Method (_CRS, 0, NotSerialized)
					{
						CreateDWordField (CRS, \_SB.PCI0.PCIE._Y10._BAS, BAS1)
						CreateDWordField (CRS, \_SB.PCI0.PCIE._Y10._LEN, LEN1)
						Store (0xE0000000, BAS1)
						Store (0x10000000, LEN1)
						Return (CRS)
					}
				}

		/* PIC */
				Device (PIC)
				{
			Name (_HID, EisaId ("PNP0000"))
			Name (_CRS, ResourceTemplate()
			{
				IO (Decode16,
				0x0020,
				0x0020,
				0x00,
				0x02,
				)
			IO (Decode16,
				0x00A0,
				0x00A0,
				0x00,
				0x02,
				)
			IRQNoFlags ()
				{2}
			})
				}

		/* Real time clock */
				Device (RTC0)
				{
					Name (_HID, EisaId ("PNP0B00"))
					Name (_CRS, ResourceTemplate ()
					{
						IO (Decode16,
				0x0070,
				0x0070,
				0x00,
				0x02)
						IRQNoFlags ()
				{8}
					})
				}

				Device (UAR1)
				{
					Name (_UID, 0x01)
					Name (_HID, EisaId ("PNP0501"))

					Method (_PRS, 0, NotSerialized)
					{
						Return (CMPR)
					}

					Name (CMPR, ResourceTemplate ()
					{
						StartDependentFn (0x00, 0x00)
						{
							IO (Decode16,0x03F8,0x03F8,0x01,0x08)
							IRQNoFlags () {4}
							DMA (Compatibility, NotBusMaster, Transfer8) {}
						}
						StartDependentFnNoPri ()
						{
							IO (Decode16,0x03F8,0x03F8,0x01,0x08)
							IRQNoFlags () {3,4,5,6,7,10,11,12}
							DMA (Compatibility, NotBusMaster, Transfer8) {}
						}
						StartDependentFnNoPri ()
						{
							IO (Decode16,0x02F8,0x02F8,0x01,0x08)
							IRQNoFlags () {3,4,5,6,7,10,11,12}
				DMA (Compatibility, NotBusMaster, Transfer8) {}
						}
						StartDependentFnNoPri ()
						{
							IO (Decode16,0x03E8,0x03E8,0x01,0x08)
							IRQNoFlags () {3,4,5,6,7,10,11,12}
				DMA (Compatibility, NotBusMaster, Transfer8) {}
						}
						StartDependentFnNoPri ()
						{
							IO (Decode16,0x02E8,0x02E8,0x01,0x08)
							IRQNoFlags () {3,4,5,6,7,10,11,12}
							DMA (Compatibility, NotBusMaster, Transfer8) {}
						}
						EndDependentFn ()
					})
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

		/* COM ports of SIO */
		Device(SIO) {
				OperationRegion (PT4E, SystemIO, 0x4E, 0x02)
				Field (PT4E, ByteAcc, NoLock, Preserve)
				{
					PO4E,   8,
					PO4F,   8
				}

				IndexField (PO4E, PO4F, ByteAcc, NoLock, Preserve)
				{
							Offset (0x07),
					ILDN,   8,
							Offset (0x28),
					SIUI,   8,
					SIUC,   8,
							Offset (0x30),
					IACT,   8,
							Offset (0x60),
					IIOH,   8,
					IIOL,   8,
							Offset (0x70),
					IINT,   8
				}

				Method (IENF, 0, NotSerialized)
				{
					Store (0x80, PO4E)
					Store (0x86, PO4E)
				}

				Method (IEXF, 0, NotSerialized)
				{
					Store (0x68, PO4E)
					Store (0x08, PO4E)
				}

				Device (COM1)
				{
					Name (_UID, 0x03)
					Name (_HID, EisaId ("PNP0501"))
					Method (_STA, 0, NotSerialized)
					{
						IENF ()
						Store (0x04, ILDN)
						Store (IACT, Local0)
						IEXF ()
						If (LEqual (Local0, 0xFF))
						{
							Return (0x00)
						}

						If (LEqual (Local0, One))
						{
							Return (0x0F)
						}
						Else
						{
							Return (0x0D)
						}
					}

					Method (_DIS, 0, NotSerialized)
					{
						IENF ()
						Store (0x04, ILDN)
						Store (Zero, IACT)
						IEXF ()
					}

					Method (_CRS, 0, NotSerialized)
					{
						Name (BFU1, ResourceTemplate ()
						{
							IO (Decode16,
								0x03F8,             // Range Minimum
								0x03F8,             // Range Maximum
								0x08,               // Alignment
								0x08,               // Length
								_Y03)
							IRQNoFlags (_Y04)
								{5}
						})
						CreateWordField (BFU1, \_SB.PCI0.ISA.SIO.COM1._CRS._Y03._MIN, IMIN)
						CreateWordField (BFU1, \_SB.PCI0.ISA.SIO.COM1._CRS._Y03._MAX, IMAX)
						CreateWordField (BFU1, \_SB.PCI0.ISA.SIO.COM1._CRS._Y04._INT, IRQ0)
						IENF ()
						Store (0x04, ILDN)
						Store (IIOH, Local0)
						ShiftLeft (Local0, 0x08, Local1)
						Store (IIOL, Local0)
						Add (Local1, Local0, Local0)
						Store (Local0, IMIN)
						Store (Local0, IMAX)
						Store (IINT, Local0)
						IEXF ()
						Store (0x01, Local1)
						ShiftLeft (Local1, Local0, IRQ0)
						Return (BFU1)
					}

					Name (_PRS, ResourceTemplate ()
					{
						StartDependentFnNoPri ()
						{
							IO (Decode16,
								0x03F8,             // Range Minimum
								0x03F8,             // Range Maximum
								0x08,               // Alignment
								0x08,               // Length
								)
							IRQNoFlags ()
								{5}
						}
						StartDependentFnNoPri ()
						{
							IO (Decode16,
								0x02F8,             // Range Minimum
								0x02F8,             // Range Maximum
								0x08,               // Alignment
								0x08,               // Length
								)
							IRQNoFlags ()
								{9}
						}
				EndDependentFn ()
					})
					Method (_SRS, 1, NotSerialized)
					{
						CreateByteField (Arg0, 0x02, IOLO)
						CreateByteField (Arg0, 0x03, IOHI)
						CreateWordField (Arg0, 0x09, IRQ0)
						IENF ()
						Store (0x04, ILDN)
						Store (Zero, IACT)
						Store (IOLO, IIOL)
						Store (IOHI, IIOH)
						FindSetRightBit (IRQ0, Local0)
						If (LGreater (Local0, 0x00))
						{
							Decrement (Local0)
						}

						Store (Local0, IINT)
						Store (One, IACT)
						IEXF ()
					}
				} /* COM1 */

				Device (COM2)
				{
					Name (_UID, 0x04)
					Name (_HID, EisaId ("PNP0501"))
					Method (_STA, 0, NotSerialized)
					{
						IENF ()
						Store (0x05, ILDN)
						Store (IACT, Local0)
						IEXF ()
						If (LEqual (Local0, 0xFF))
						{
							Return (0x00)
						}

						If (LEqual (Local0, One))
						{
							Return (0x0F)
						}
						Else
						{
							Return (0x0D)
						}
					}

					Method (_DIS, 0, NotSerialized)
					{
						IENF ()
						Store (0x05, ILDN)
						Store (Zero, IACT)
						IEXF ()
					}

					Method (_CRS, 0, NotSerialized)
					{
						Name (BFU1, ResourceTemplate ()
						{
							IO (Decode16,
								0x03F8,             // Range Minimum
								0x03F8,             // Range Maximum
								0x08,               // Alignment
								0x08,               // Length
								_Y05)
							IRQNoFlags (_Y06)
								{9}
						})
						CreateWordField (BFU1, \_SB.PCI0.ISA.SIO.COM2._CRS._Y05._MIN, IMIN)
						CreateWordField (BFU1, \_SB.PCI0.ISA.SIO.COM2._CRS._Y05._MAX, IMAX)
						CreateWordField (BFU1, \_SB.PCI0.ISA.SIO.COM2._CRS._Y06._INT, IRQ0)
						IENF ()
						Store (0x05, ILDN)
						Store (IIOH, Local0)
						ShiftLeft (Local0, 0x08, Local1)
						Store (IIOL, Local0)
						Add (Local1, Local0, Local0)
						Store (Local0, IMIN)
						Store (Local0, IMAX)
						Store (IINT, Local0)
						IEXF ()
						Store (0x01, Local1)
						ShiftLeft (Local1, Local0, IRQ0)
						Return (BFU1)
					}

					Name (_PRS, ResourceTemplate ()
					{
						StartDependentFnNoPri ()
						{
							IO (Decode16,
								0x03F8,             // Range Minimum
								0x03F8,             // Range Maximum
								0x08,               // Alignment
								0x08,               // Length
								)
							IRQNoFlags ()
								{5}
						}
						StartDependentFnNoPri ()
						{
							IO (Decode16,
								0x02F8,             // Range Minimum
								0x02F8,             // Range Maximum
								0x08,               // Alignment
								0x08,               // Length
								)
							IRQNoFlags ()
								{9}
						}
						EndDependentFn ()
					})
					Method (_SRS, 1, NotSerialized)
					{
						CreateByteField (Arg0, 0x02, IOLO)
						CreateByteField (Arg0, 0x03, IOHI)
						CreateWordField (Arg0, 0x09, IRQ0)
						IENF ()
						Store (0x05, ILDN)
						Store (Zero, IACT)
						Store (IOLO, IIOL)
						Store (IOHI, IIOH)
						FindSetRightBit (IRQ0, Local0)
						If (LGreater (Local0, 0x00))
						{
							Decrement (Local0)
						}

						Store (Local0, IINT)
						Store (One, IACT)
						IEXF ()
					}
				} /* COM2 */
		} /* Device SIO */
		} /* Device ISA */
	} /* Device PCI 0*/
	} /* Scope SB */

	OperationRegion (_SB.PCI0.ISA.PIX0, PCI_Config, 0x60, 0x0C)
	Field (\_SB.PCI0.ISA.PIX0, ByteAcc, NoLock, Preserve)
	{
		PIRA,   8,
		PIRB,   8,
		PIRC,   8,
		PIRD,   8,
				Offset (0x08),
		PIRE,   8,
		PIRF,   8,
		PIRG,   8,
		PIRH,   8
	}

	Scope (_SB)
	{
		Name (BUFA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared, _Y1C)
				{15}
		})
		CreateWordField (BUFA, \_SB._Y1C._INT, IRA0)
		Device (LNKA)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x01)
			Method (_STA, 0, NotSerialized)
			{
				And (PIRA, 0x80, Local0)
				If (Local0)
				{
					Return (0x09)
				}
				Else
				{
					Return (0x0B)
				}
			}

			Method (_PRS, 0, NotSerialized)
			{
				Return (PRSA)
			}

			Method (_DIS, 0, NotSerialized)
			{
				Or (PIRA, 0x80, PIRA)
			}

			Method (_CRS, 0, NotSerialized)
			{
				And (PIRA, 0x0F, Local0)
				ShiftLeft (0x01, Local0, IRA0)
				Return (BUFA)
			}

			Method (_SRS, 1, NotSerialized)
			{
				CreateWordField (Arg0, 0x01, IRA)
				FindSetRightBit (IRA, Local0)
				Decrement (Local0)
				Store (Local0, PIRA)
			}
		}

		Device (LNKB)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x02)
			Method (_STA, 0, NotSerialized)
			{
				And (PIRB, 0x80, Local0)
				If (Local0)
				{
					Return (0x09)
				}
				Else
				{
					Return (0x0B)
				}
			}

			Method (_PRS, 0, NotSerialized)
			{
				Return (PRSB)
			}

			Method (_DIS, 0, NotSerialized)
			{
				Or (PIRB, 0x80, PIRB)
			}

			Method (_CRS, 0, NotSerialized)
			{
				And (PIRB, 0x0F, Local0)
				ShiftLeft (0x01, Local0, IRA0)
				Return (BUFA)
			}

			Method (_SRS, 1, NotSerialized)
			{
				CreateWordField (Arg0, 0x01, IRA)
				FindSetRightBit (IRA, Local0)
				Decrement (Local0)
				Store (Local0, PIRB)
			}
		}

		Device (LNKC)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x03)
			Method (_STA, 0, NotSerialized)
			{
				And (PIRC, 0x80, Local0)
				If (Local0)
				{
					Return (0x09)
				}
				Else
				{
					Return (0x0B)
				}
			}

			Method (_PRS, 0, NotSerialized)
			{
				Return (PRSC)
			}

			Method (_DIS, 0, NotSerialized)
			{
				Or (PIRC, 0x80, PIRC)
			}

			Method (_CRS, 0, NotSerialized)
			{
				And (PIRC, 0x0F, Local0)
				ShiftLeft (0x01, Local0, IRA0)
				Return (BUFA)
			}

			Method (_SRS, 1, NotSerialized)
			{
				CreateWordField (Arg0, 0x01, IRA)
				FindSetRightBit (IRA, Local0)
				Decrement (Local0)
				Store (Local0, PIRC)
			}
		}

		Device (LNKD)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x04)
			Method (_STA, 0, NotSerialized)
			{
				And (PIRD, 0x80, Local0)
				If (Local0)
				{
					Return (0x09)
				}
				Else
				{
					Return (0x0B)
				}
			}

			Method (_PRS, 0, NotSerialized)
			{
				Return (PRSD)
			}

			Method (_DIS, 0, NotSerialized)
			{
				Or (PIRD, 0x80, PIRD)
			}

			Method (_CRS, 0, NotSerialized)
			{
				And (PIRD, 0x0F, Local0)
				ShiftLeft (0x01, Local0, IRA0)
				Return (BUFA)
			}

			Method (_SRS, 1, NotSerialized)
			{
				CreateWordField (Arg0, 0x01, IRA)
				FindSetRightBit (IRA, Local0)
				Decrement (Local0)
				Store (Local0, PIRD)
			}
		}

		Device (LNKE)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x05)
			Method (_STA, 0, NotSerialized)
			{
				And (PIRE, 0x80, Local0)
				If (Local0)
				{
					Return (0x09)
				}
				Else
				{
					Return (0x0B)
				}
			}

			Method (_PRS, 0, NotSerialized)
			{
				Return (PRSE)
			}

			Method (_DIS, 0, NotSerialized)
			{
				Or (PIRE, 0x80, PIRE)
			}

			Method (_CRS, 0, NotSerialized)
			{
				And (PIRE, 0x0F, Local0)
				ShiftLeft (0x01, Local0, IRA0)
				Return (BUFA)
			}

			Method (_SRS, 1, NotSerialized)
			{
				CreateWordField (Arg0, 0x01, IRA)
				FindSetRightBit (IRA, Local0)
				Decrement (Local0)
				Store (Local0, PIRE)
			}
		}

		Device (LNKF)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x06)
			Method (_STA, 0, NotSerialized)
			{
				And (PIRF, 0x80, Local0)
				If (Local0)
				{
					Return (0x09)
				}
				Else
				{
					Return (0x0B)
				}
			}

			Method (_PRS, 0, NotSerialized)
			{
				Return (PRSF)
			}

			Method (_DIS, 0, NotSerialized)
			{
				Or (PIRF, 0x80, PIRF)
			}

			Method (_CRS, 0, NotSerialized)
			{
				And (PIRF, 0x0F, Local0)
				ShiftLeft (0x01, Local0, IRA0)
				Return (BUFA)
			}

			Method (_SRS, 1, NotSerialized)
			{
				CreateWordField (Arg0, 0x01, IRA)
				FindSetRightBit (IRA, Local0)
				Decrement (Local0)
				Store (Local0, PIRF)
			}
		}

		Device (LNKG)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x07)
			Method (_STA, 0, NotSerialized)
			{
				And (PIRG, 0x80, Local0)
				If (Local0)
				{
					Return (0x09)
				}
				Else
				{
					Return (0x0B)
				}
			}

			Method (_PRS, 0, NotSerialized)
			{
				Return (PRSG)
			}

			Method (_DIS, 0, NotSerialized)
			{
				Or (PIRG, 0x80, PIRG)
			}

			Method (_CRS, 0, NotSerialized)
			{
				And (PIRG, 0x0F, Local0)
				ShiftLeft (0x01, Local0, IRA0)
				Return (BUFA)
			}

			Method (_SRS, 1, NotSerialized)
			{
				CreateWordField (Arg0, 0x01, IRA)
				FindSetRightBit (IRA, Local0)
				Decrement (Local0)
				Store (Local0, PIRG)
			}
		}

		Device (LNKH)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x08)
			Method (_STA, 0, NotSerialized)
			{
				And (PIRH, 0x80, Local0)
				If (Local0)
				{
					Return (0x09)
				}
				Else
				{
					Return (0x0B)
				}
			}

			Method (_PRS, 0, NotSerialized)
			{
				Return (PRSH)
			}

			Method (_DIS, 0, NotSerialized)
			{
				Or (PIRH, 0x80, PIRH)
			}

			Method (_CRS, 0, NotSerialized)
			{
				And (PIRH, 0x0F, Local0)
				ShiftLeft (0x01, Local0, IRA0)
				Return (BUFA)
			}

			Method (_SRS, 1, NotSerialized)
			{
				CreateWordField (Arg0, 0x01, IRA)
				FindSetRightBit (IRA, Local0)
				Decrement (Local0)
				Store (Local0, PIRH)
			}
		}
	}
}


