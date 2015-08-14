/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2009 Advanced Micro Devices, Inc.
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

Scope(\) {
	Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)	/* Base address of PCIe config space */
	Name(HPBA, 0xFED00000)			/* Base address of HPET table */

	/* PIC IRQ mapping registers, C00h-C01h */
	OperationRegion(PRQM, SystemIO, 0x00000C00, 0x00000002)
		Field(PRQM, ByteAcc, NoLock, Preserve) {
		PRQI, 0x00000008,
		PRQD, 0x00000008,  /* Offset: 1h */
	}
	IndexField(PRQI, PRQD, ByteAcc, NoLock, Preserve) {
		PINA, 0x00000008,	/* Index 0 */
		PINB, 0x00000008,	/* Index 1 */
		PINC, 0x00000008,	/* Index 2 */
		PIND, 0x00000008,	/* Index 3 */
		AINT, 0x00000008,	/* Index 4 */
		SINT, 0x00000008,	/* Index 5 */
		    , 0x00000008,	/* Index 6 */
		AAUD, 0x00000008,	/* Index 7 */
		AMOD, 0x00000008,	/* Index 8 */
		PINE, 0x00000008,	/* Index 9 */
		PINF, 0x00000008,	/* Index A */
		PING, 0x00000008,	/* Index B */
		PINH, 0x00000008,	/* Index C */
	}

	/* PCI Error control register */
	OperationRegion(PERC, SystemIO, 0x00000C14, 0x00000001)
		Field(PERC, ByteAcc, NoLock, Preserve) {
		SENS, 0x00000001,
		PENS, 0x00000001,
		SENE, 0x00000001,
		PENE, 0x00000001,
	}

	Scope(\_SB) {
		/* PCIe Configuration Space for 16 busses */
		OperationRegion(PCFG, SystemMemory, PCBA, 0x01000000) /* Each bus consumes 1MB */
			Field(PCFG, ByteAcc, NoLock, Preserve) {
			/* Byte offsets are computed using the following technique:
			 * ((bus number + 1) * ((device number * 8) * 4096)) + register offset
			 * The 8 comes from 8 functions per device, and 4096 bytes per function config space
			*/
			Offset(0x00088024),	/* Byte offset to SATA register 24h - Bus 0, Device 17, Function 0 */
			STB5, 32,
			Offset(0x00098042),	/* Byte offset to OHCI0 register 42h - Bus 0, Device 19, Function 0 */
			PT0D, 1,
			PT1D, 1,
			PT2D, 1,
			PT3D, 1,
			PT4D, 1,
			PT5D, 1,
			PT6D, 1,
			PT7D, 1,
			PT8D, 1,
			PT9D, 1,
			Offset(0x000A0004),	/* Byte offset to SMBUS	register 4h - Bus 0, Device 20, Function 0 */
			SBIE, 1,
			SBME, 1,
			Offset(0x000A0008),	/* Byte offset to SMBUS	register 8h - Bus 0, Device 20, Function 0 */
			SBRI, 8,
			Offset(0x000A0014),	/* Byte offset to SMBUS	register 14h - Bus 0, Device 20, Function 0 */
			SBB1, 32,
			Offset(0x000A0078),	/* Byte offset to SMBUS	register 78h - Bus 0, Device 20, Function 0 */
			,14,
			P92E, 1,		/* Port92 decode enable */
		}

		OperationRegion(SB5, SystemMemory, STB5, 0x1000)
			Field(SB5, AnyAcc, NoLock, Preserve){
			/* Port 0 */
			Offset(0x120),		/* Port 0 Task file status */
			P0ER, 1,
			, 2,
			P0DQ, 1,
			, 3,
			P0BY, 1,
			Offset(0x128),		/* Port 0 Serial ATA status */
			P0DD, 4,
			, 4,
			P0IS, 4,
			Offset(0x12C),		/* Port 0 Serial ATA control */
			P0DI, 4,
			Offset(0x130),		/* Port 0 Serial ATA error */
			, 16,
			P0PR, 1,

			/* Port 1 */
			offset(0x1A0),		/* Port 1 Task file status */
			P1ER, 1,
			, 2,
			P1DQ, 1,
			, 3,
			P1BY, 1,
			Offset(0x1A8),		/* Port 1 Serial ATA status */
			P1DD, 4,
			, 4,
			P1IS, 4,
			Offset(0x1AC),		/* Port 1 Serial ATA control */
			P1DI, 4,
			Offset(0x1B0),		/* Port 1 Serial ATA error */
			, 16,
			P1PR, 1,

			/* Port 2 */
			Offset(0x220),		/* Port 2 Task file status */
			P2ER, 1,
			, 2,
			P2DQ, 1,
			, 3,
			P2BY, 1,
			Offset(0x228),		/* Port 2 Serial ATA status */
			P2DD, 4,
			, 4,
			P2IS, 4,
			Offset(0x22C),		/* Port 2 Serial ATA control */
			P2DI, 4,
			Offset(0x230),		/* Port 2 Serial ATA error */
			, 16,
			P2PR, 1,

			/* Port 3 */
			Offset(0x2A0),		/* Port 3 Task file status */
			P3ER, 1,
			, 2,
			P3DQ, 1,
			, 3,
			P3BY, 1,
			Offset(0x2A8),		/* Port 3 Serial ATA status */
			P3DD, 4,
			, 4,
			P3IS, 4,
			Offset(0x2AC),		/* Port 3 Serial ATA control */
			P3DI, 4,
			Offset(0x2B0),		/* Port 3 Serial ATA error */
			, 16,
			P3PR, 1,
		}

		Method(CIRQ, 0x00, NotSerialized){
			Store(0, PINA)
			Store(0, PINB)
			Store(0, PINC)
			Store(0, PIND)
			Store(0, PINE)
			Store(0, PINF)
			Store(0, PING)
			Store(0, PINH)
		}

		/* set "A", 8259 interrupts */
		Name (PRSA, ResourceTemplate () {
			IRQ(Level, ActiveLow, Exclusive) {4, 7, 10, 11, 12, 14, 15}
		})

		Method (CRSA, 1, Serialized) {
			Name (LRTL, ResourceTemplate() {
				IRQ(Level, ActiveLow, Shared) {15}
			})
			CreateWordField(LRTL, 1, LIRQ)
			ShiftLeft(1, Arg0, LIRQ)
			Return (LRTL)
		}

		Method (SRSA, 1, Serialized) {
			CreateWordField(Arg0, 1, LIRQ)
			FindSetRightBit(LIRQ, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Return (Local0)
		}

		Device(LNKA) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 1)
			Method(_STA, 0) {
				if (PINA) {
					Return(0x0B) /* LNKA is invisible */
				} else {
					Return(0x09) /* LNKA is disabled */
				}
			}
			Method(_DIS, 0) {
				Store(0, PINA)
			}
			Method(_PRS, 0) {
				Return (PRSA)
			}
			Method (_CRS, 0, Serialized) {
				Return (CRSA(PINA))
			}
			Method (_SRS, 1, Serialized) {
				Store (SRSA(Arg0), PINA)
			}
		}

		Device(LNKB) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 2)
			Method(_STA, 0) {
				if (PINB) {
					Return(0x0B) /* LNKB is invisible */
				} else {
					Return(0x09) /* LNKB is disabled */
				}
			}
			Method(_DIS, 0) {
				Store(0, PINB)
			}
			Method(_PRS, 0) {
				Return (PRSA)
			}
			Method (_CRS, 0, Serialized) {
				Return (CRSA(PINB))
			}
			Method (_SRS, 1, Serialized) {
				Store (SRSA(Arg0), PINB)
			}
		}

		Device(LNKC) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 3)
			Method(_STA, 0) {
				if (PINC) {
					Return(0x0B) /* LNKC is invisible */
				} else {
					Return(0x09) /* LNKC is disabled */
				}
			}
			Method(_DIS, 0) {
				Store(0, PINC)
			}
			Method(_PRS, 0) {
				Return (PRSA)
			}
			Method (_CRS, 0, Serialized) {
				Return (CRSA(PINC))
			}
			Method (_SRS, 1, Serialized) {
				Store (SRSA(Arg0), PINC)
			}
		}

		Device(LNKD) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 4)
			Method(_STA, 0) {
				if (PIND) {
					Return(0x0B) /* LNKD is invisible */
				} else {
					Return(0x09) /* LNKD is disabled */
				}
			}
			Method(_DIS, 0) {
				Store(0, PIND)
			}
			Method(_PRS, 0) {
				Return (PRSA)
			}
			Method (_CRS, 0, Serialized) {
				Return (CRSA(PIND))
			}
			Method (_SRS, 1, Serialized) {
				Store (SRSA(Arg0), PIND)
			}
		}

		Device(LNKE) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 5)
			Method(_STA, 0) {
				if (PINE) {
					Return(0x0B) /* LNKE is invisible */
				} else {
					Return(0x09) /* LNKE is disabled */
				}
			}
			Method(_DIS, 0) {
				Store(0, PINE)
			}
			Method(_PRS, 0) {
				Return (PRSA)
			}
			Method (_CRS, 0, Serialized) {
				Return (CRSA(PINE))
			}
			Method (_SRS, 1, Serialized) {
				Store (SRSA(Arg0), PINE)
			}
		}

		Device(LNKF) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 6)
			Method(_STA, 0) {
				if (PINF) {
					Return(0x0B) /* LNKF is invisible */
				} else {
					Return(0x09) /* LNKF is disabled */
				}
			}
			Method(_DIS, 0) {
				Store(0, PINF)
			}
			Method(_PRS, 0) {
				Return (PRSA)
			}
			Method (_CRS, 0, Serialized) {
				Return (CRSA(PINF))
			}
			Method (_SRS, 1, Serialized) {
				Store (SRSA(Arg0), PINF)
			}
		}

		Device(LNKG) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 7)
			Method(_STA, 0) {
				if (PING) {
					Return(0x0B) /* LNKG is invisible */
				} else {
					Return(0x09) /* LNKG is disabled */
				}
			}
			Method(_DIS, 0) {
				Store(0, PING)
			}
			Method(_PRS, 0) {
				Return (PRSA)
			}
			Method (_CRS, 0, Serialized) {
				Return (CRSA(PING))
			}
			Method (_SRS, 1, Serialized) {
				Store (SRSA(Arg0), PING)
			}
		}

		Device(LNKH) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 8)
			Method(_STA, 0) {
				if (PINH) {
					Return(0x0B) /* LNKH is invisible */
				} else {
					Return(0x09) /* LNKH is disabled */
				}
			}
			Method(_DIS, 0) {
				Store(0, PINH)
			}
			Method(_PRS, 0) {
				Return (PRSA)
			}
			Method (_CRS, 0, Serialized) {
				Return (CRSA(PINH))
			}
			Method (_SRS, 1, Serialized) {
				Store (SRSA(Arg0), PINH)
			}
		}

	}   /* End Scope(_SB)  */

}  /* End Scope(/)  */
