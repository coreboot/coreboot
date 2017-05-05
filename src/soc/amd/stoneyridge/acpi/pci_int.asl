/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

	/* PCIe Configuration Space for CONFIG_MMCONF_BUS_NUMBER busses */
	OperationRegion(PCFG, SystemMemory, PCBA, PCLN) /* Each bus consumes 1MB */
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
		Offset(0x000A0004),	/* Byte offset to SMBUS register 4h - Bus 0, Device 20, Function 0 */
		SBIE, 1,
		SBME, 1,
		Offset(0x000A0008),	/* Byte offset to SMBUS register 8h - Bus 0, Device 20, Function 0 */
		SBRI, 8,
		Offset(0x000A0014),	/* Byte offset to SMBUS register 14h - Bus 0, Device 20, Function 0 */
		SBB1, 32,
		Offset(0x000A0078),	/* Byte offset to SMBUS register 78h - Bus 0, Device 20, Function 0 */
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

	Method(_PIC, 0x01, NotSerialized)
	{
		If (Arg0)
		{
			\_SB.CIRQ()
		}
		Store(Arg0, PMOD)
	}

	Method(CIRQ, 0x00, NotSerialized){
	}

	Name(IRQB, ResourceTemplate(){
		IRQ(Level,ActiveLow,Shared){15}
	})

	Name(IRQP, ResourceTemplate(){
		IRQ(Level,ActiveLow,Exclusive){3, 4, 5, 7, 10, 11, 12, 15}
	})

	Name(PITF, ResourceTemplate(){
		IRQ(Level,ActiveLow,Exclusive){9}
	})

	Device(INTA) {
		Name(_HID, EISAID("PNP0C0F"))
		Name(_UID, 1)

		Method(_STA, 0) {
			if (PIRA) {
				Return(0x0B) /* sata is invisible */
			} else {
				Return(0x09) /* sata is disabled */
			}
		} /* End Method(_SB.INTA._STA) */

		Method(_DIS ,0) {
			/* DBGO("\\_SB\\LNKA\\_DIS\n") */
		} /* End Method(_SB.INTA._DIS) */

		Method(_PRS ,0) {
			/* DBGO("\\_SB\\LNKA\\_PRS\n") */
			Return(IRQP)
		} /* Method(_SB.INTA._PRS) */

		Method(_CRS ,0) {
			/* DBGO("\\_SB\\LNKA\\_CRS\n") */
			CreateWordField(IRQB, 0x1, IRQN)
			ShiftLeft(1, PIRA, IRQN)
			Return(IRQB)
		} /* Method(_SB.INTA._CRS) */

		Method(_SRS, 1) {
			/* DBGO("\\_SB\\LNKA\\_SRS\n") */
			CreateWordField(ARG0, 1, IRQM)

			/* Use lowest available IRQ */
			FindSetRightBit(IRQM, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Store(Local0, PIRA)
		} /* End Method(_SB.INTA._SRS) */
	} /* End Device(INTA) */

	Device(INTB) {
		Name(_HID, EISAID("PNP0C0F"))
		Name(_UID, 2)

		Method(_STA, 0) {
			if (PIRB) {
				Return(0x0B) /* sata is invisible */
			} else {
				Return(0x09) /* sata is disabled */
			}
		} /* End Method(_SB.INTB._STA) */

		Method(_DIS ,0) {
			/* DBGO("\\_SB\\LNKB\\_DIS\n") */
		} /* End Method(_SB.INTB._DIS) */

		Method(_PRS ,0) {
			/* DBGO("\\_SB\\LNKB\\_PRS\n") */
			Return(IRQP)
		} /* Method(_SB.INTB._PRS) */

		Method(_CRS ,0) {
			/* DBGO("\\_SB\\LNKB\\_CRS\n") */
			CreateWordField(IRQB, 0x1, IRQN)
			ShiftLeft(1, PIRB, IRQN)
			Return(IRQB)
		} /* Method(_SB.INTB._CRS) */

		Method(_SRS, 1) {
			/* DBGO("\\_SB\\LNKB\\_CRS\n") */
			CreateWordField(ARG0, 1, IRQM)

			/* Use lowest available IRQ */
			FindSetRightBit(IRQM, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Store(Local0, PIRB)
		} /* End Method(_SB.INTB._SRS) */
	} /* End Device(INTB)  */

	Device(INTC) {
		Name(_HID, EISAID("PNP0C0F"))
		Name(_UID, 3)

		Method(_STA, 0) {
			if (PIRC) {
				Return(0x0B) /* sata is invisible */
			} else {
				Return(0x09) /* sata is disabled */
			}
		} /* End Method(_SB.INTC._STA) */

		Method(_DIS ,0) {
			/* DBGO("\\_SB\\LNKC\\_DIS\n") */
		} /* End Method(_SB.INTC._DIS) */

		Method(_PRS ,0) {
			/* DBGO("\\_SB\\LNKC\\_PRS\n") */
			Return(IRQP)
		} /* Method(_SB.INTC._PRS) */

		Method(_CRS ,0) {
			/* DBGO("\\_SB\\LNKC\\_CRS\n") */
			CreateWordField(IRQB, 0x1, IRQN)
			ShiftLeft(1, PIRC, IRQN)
			Return(IRQB)
		} /* Method(_SB.INTC._CRS) */

		Method(_SRS, 1) {
			/* DBGO("\\_SB\\LNKC\\_CRS\n") */
			CreateWordField(ARG0, 1, IRQM)

			/* Use lowest available IRQ */
			FindSetRightBit(IRQM, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Store(Local0, PIRC)
		} /* End Method(_SB.INTC._SRS) */
	} /* End Device(INTC)  */

	Device(INTD) {
		Name(_HID, EISAID("PNP0C0F"))
		Name(_UID, 4)

		Method(_STA, 0) {
			if (PIRD) {
				Return(0x0B) /* sata is invisible */
			} else {
				Return(0x09) /* sata is disabled */
			}
		} /* End Method(_SB.INTD._STA) */

		Method(_DIS ,0) {
			/* DBGO("\\_SB\\LNKD\\_DIS\n") */
		} /* End Method(_SB.INTD._DIS) */

		Method(_PRS ,0) {
			/* DBGO("\\_SB\\LNKD\\_PRS\n") */
			Return(IRQP)
		} /* Method(_SB.INTD._PRS) */

		Method(_CRS ,0) {
			/* DBGO("\\_SB\\LNKD\\_CRS\n") */
			CreateWordField(IRQB, 0x1, IRQN)
			ShiftLeft(1, PIRD, IRQN)
			Return(IRQB)
		} /* Method(_SB.INTD._CRS) */

		Method(_SRS, 1) {
			/* DBGO("\\_SB\\LNKD\\_CRS\n") */
			CreateWordField(ARG0, 1, IRQM)

			/* Use lowest available IRQ */
			FindSetRightBit(IRQM, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Store(Local0, PIRD)
		} /* End Method(_SB.INTD._SRS) */
	} /* End Device(INTD)  */

	Device(INTE) {
		Name(_HID, EISAID("PNP0C0F"))
		Name(_UID, 5)

		Method(_STA, 0) {
			if (PIRE) {
				Return(0x0B) /* sata is invisible */
			} else {
				Return(0x09) /* sata is disabled */
			}
		} /* End Method(_SB.INTE._STA) */

		Method(_DIS ,0) {
			/* DBGO("\\_SB\\LNKE\\_DIS\n") */
		} /* End Method(_SB.INTE._DIS) */

		Method(_PRS ,0) {
			/* DBGO("\\_SB\\LNKE\\_PRS\n") */
			Return(IRQP)
		} /* Method(_SB.INTE._PRS) */

		Method(_CRS ,0) {
			/* DBGO("\\_SB\\LNKE\\_CRS\n") */
			CreateWordField(IRQB, 0x1, IRQN)
			ShiftLeft(1, PIRE, IRQN)
			Return(IRQB)
		} /* Method(_SB.INTE._CRS) */

		Method(_SRS, 1) {
			/* DBGO("\\_SB\\LNKE\\_CRS\n") */
			CreateWordField(ARG0, 1, IRQM)

			/* Use lowest available IRQ */
			FindSetRightBit(IRQM, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Store(Local0, PIRE)
		} /* End Method(_SB.INTE._SRS) */
	} /* End Device(INTE)  */

	Device(INTF) {
		Name(_HID, EISAID("PNP0C0F"))
		Name(_UID, 6)

		Method(_STA, 0) {
			if (PIRF) {
				Return(0x0B) /* sata is invisible */
			} else {
				Return(0x09) /* sata is disabled */
			}
		} /* End Method(_SB.INTF._STA) */

		Method(_DIS ,0) {
			/* DBGO("\\_SB\\LNKF\\_DIS\n") */
		} /* End Method(_SB.INTF._DIS) */

		Method(_PRS ,0) {
			/* DBGO("\\_SB\\LNKF\\_PRS\n") */
			Return(PITF)
		} /* Method(_SB.INTF._PRS) */

		Method(_CRS ,0) {
			/* DBGO("\\_SB\\LNKF\\_CRS\n") */
			CreateWordField(IRQB, 0x1, IRQN)
			ShiftLeft(1, PIRF, IRQN)
			Return(IRQB)
		} /* Method(_SB.INTF._CRS) */

		Method(_SRS, 1) {
			/* DBGO("\\_SB\\LNKF\\_CRS\n") */
			CreateWordField(ARG0, 1, IRQM)

			/* Use lowest available IRQ */
			FindSetRightBit(IRQM, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Store(Local0, PIRF)
		} /*  End Method(_SB.INTF._SRS) */
	} /* End Device(INTF)  */

	Device(INTG) {
		Name(_HID, EISAID("PNP0C0F"))
		Name(_UID, 7)

		Method(_STA, 0) {
			if (PIRG) {
				Return(0x0B) /* sata is invisible */
			} else {
				Return(0x09) /* sata is disabled */
			}
		} /* End Method(_SB.INTG._STA)  */

		Method(_DIS ,0) {
			/* DBGO("\\_SB\\LNKG\\_DIS\n") */
		} /* End Method(_SB.INTG._DIS)  */

		Method(_PRS ,0) {
			/* DBGO("\\_SB\\LNKG\\_PRS\n") */
			Return(IRQP)
		} /* Method(_SB.INTG._CRS)  */

		Method(_CRS ,0) {
			/* DBGO("\\_SB\\LNKG\\_CRS\n") */
			CreateWordField(IRQB, 0x1, IRQN)
			ShiftLeft(1, PIRG, IRQN)
			Return(IRQB)
		} /* Method(_SB.INTG._CRS)  */

		Method(_SRS, 1) {
			/* DBGO("\\_SB\\LNKG\\_CRS\n") */
			CreateWordField(ARG0, 1, IRQM)

			/* Use lowest available IRQ */
			FindSetRightBit(IRQM, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Store(Local0, PIRG)
		} /* End Method(_SB.INTG._SRS)  */
	} /* End Device(INTG)  */

	Device(INTH) {
		Name(_HID, EISAID("PNP0C0F"))
		Name(_UID, 8)

		Method(_STA, 0) {
			if (PIRH) {
				Return(0x0B) /* sata is invisible */
			} else {
				Return(0x09) /* sata is disabled */
			}
		} /* End Method(_SB.INTH._STA)  */

		Method(_DIS ,0) {
			/* DBGO("\\_SB\\LNKH\\_DIS\n") */
		} /* End Method(_SB.INTH._DIS)  */

		Method(_PRS ,0) {
			/* DBGO("\\_SB\\LNKH\\_PRS\n") */
			Return(IRQP)
		} /* Method(_SB.INTH._CRS)  */

		Method(_CRS ,0) {
			/* DBGO("\\_SB\\LNKH\\_CRS\n") */
			CreateWordField(IRQB, 0x1, IRQN)
			ShiftLeft(1, PIRH, IRQN)
			Return(IRQB)
		} /* Method(_SB.INTH._CRS)  */

		Method(_SRS, 1) {
			/* DBGO("\\_SB\\LNKH\\_CRS\n") */
			CreateWordField(ARG0, 1, IRQM)

			/* Use lowest available IRQ */
			FindSetRightBit(IRQM, Local0)
			if (Local0) {
				Decrement(Local0)
			}
			Store(Local0, PIRH)
		} /* End Method(_SB.INTH._SRS)  */
	} /* End Device(INTH)   */
