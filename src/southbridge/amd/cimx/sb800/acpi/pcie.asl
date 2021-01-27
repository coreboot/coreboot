/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\) {
	/* PCI IRQ mapping registers, C00h-C01h. */
	OperationRegion(PRQM, SystemIO, 0x00000C00, 0x00000002)
		Field(PRQM, ByteAcc, NoLock, Preserve) {
		PRQI, 0x00000008,
		PRQD, 0x00000008,  /* Offset: 1h */
	}
	IndexField(PRQI, PRQD, ByteAcc, NoLock, Preserve) {
		PIRA, 0x00000008,	/* Index 0 */
		PIRB, 0x00000008,	/* Index 1 */
		PIRC, 0x00000008,	/* Index 2 */
		PIRD, 0x00000008,	/* Index 3 */
		PIRE, 0x00000008,	/* Index 4 */
		PIRF, 0x00000008,	/* Index 5 */
		PIRG, 0x00000008,	/* Index 6 */
		PIRH, 0x00000008,	/* Index 7 */
	}

	/* PCI Error control register */
	OperationRegion(PERC, SystemIO, 0x00000C14, 0x00000001)
		Field(PERC, ByteAcc, NoLock, Preserve) {
		SENS, 0x00000001,
		PENS, 0x00000001,
		SENE, 0x00000001,
		PENE, 0x00000001,
	}
}

Scope(\_SB) {
	/* PCIe Configuration Space for 16 busses */
	OperationRegion(PCFG, SystemMemory, PCBA, 0x01000000) /* Each bus consumes 1MB */
		Field(PCFG, ByteAcc, NoLock, Preserve) {
		/* Byte offsets are computed using the following technique:
		 * ((bus number + 1) * ((device number * 8) * 4096)) + register offset
		 * The 8 comes from 8 functions per device, and 4096 bytes per function config space
		*/
		Offset(0x00088024),	/* SATA reg 24h Bus 0, Device 17, Function 0 */
		STB5, 32,
		Offset(0x00098042),	/* OHCI0 reg 42h - Bus 0, Device 19, Function 0 */
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
		Offset(0x000A0004),	/* SMBUS reg 4h - Bus 0, Device 20, Function 0 */
		SBIE, 1,
		SBME, 1,
		Offset(0x000A0008),	/* SMBUS reg 8h - Bus 0, Device 20, Function 0 */
		SBRI, 8,
		Offset(0x000A0014),	/* SMBUS reg 14h - Bus 0, Device 20, Function 0 */
		SBB1, 32,
		Offset(0x000A0078),	/* SMBUS reg 78h - Bus 0, Device 20, Function 0 */
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
			PICM = Arg0
		}

		Method(CIRQ, 0x00, NotSerialized){
			PIRA = 0
			PIRB = 0
			PIRC = 0
			PIRD = 0
			PIRE = 0
			PIRF = 0
			PIRG = 0
			PIRH = 0
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
					Return (0x0b) /* sata is invisible */
				} else {
					Return (0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTA._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKA\\_DIS\n") */
				PIRA = 0
			} /* End Method(_SB.INTA._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKA\\_PRS\n") */
				Return (IRQP)
			} /* Method(_SB.INTA._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKA\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				IRQN = 1 << PIRA
				Return (IRQB)
			} /* Method(_SB.INTA._CRS) */

			Method(_SRS, 1) {
				/* DBGO("\\_SB\\LNKA\\_SRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Local0--
				}
				PIRA = Local0
			} /* End Method(_SB.INTA._SRS) */
		} /* End Device(INTA) */

		Device(INTB) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 2)

			Method(_STA, 0) {
				if (PIRB) {
					Return (0x0b) /* sata is invisible */
				} else {
					Return (0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTB._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKB\\_DIS\n") */
				PIRB = 0
			} /* End Method(_SB.INTB._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKB\\_PRS\n") */
				Return (IRQP)
			} /* Method(_SB.INTB._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKB\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				IRQN = 1 << PIRB
				Return (IRQB)
			} /* Method(_SB.INTB._CRS) */

			Method(_SRS, 1) {
				/* DBGO("\\_SB\\LNKB\\_CRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Local0--
				}
				PIRB = Local0
			} /* End Method(_SB.INTB._SRS) */
		} /* End Device(INTB)  */

		Device(INTC) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 3)

			Method(_STA, 0) {
				if (PIRC) {
					Return (0x0b) /* sata is invisible */
				} else {
					Return (0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTC._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKC\\_DIS\n") */
				PIRC = 0
			} /* End Method(_SB.INTC._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKC\\_PRS\n") */
				Return(IRQP)
			} /* Method(_SB.INTC._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKC\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				IRQN = 1 << PIRC
				Return (IRQB)
			} /* Method(_SB.INTC._CRS) */

			Method(_SRS, 1) {
				/* DBGO("\\_SB\\LNKC\\_CRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Local0--
				}
				PIRC = Local0
			} /* End Method(_SB.INTC._SRS) */
		} /* End Device(INTC)  */

		Device(INTD) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 4)

			Method(_STA, 0) {
				if (PIRD) {
					Return (0x0b) /* sata is invisible */
				} else {
					Return (0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTD._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKD\\_DIS\n") */
				PIRD = 0
			} /* End Method(_SB.INTD._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKD\\_PRS\n") */
				Return (IRQP)
			} /* Method(_SB.INTD._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKD\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				IRQN = 1 << PIRD
				Return (IRQB)
			} /* Method(_SB.INTD._CRS) */

			Method(_SRS, 1) {
				/* DBGO("\\_SB\\LNKD\\_CRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Local0--
				}
				PIRD = Local0
			} /* End Method(_SB.INTD._SRS) */
		} /* End Device(INTD)  */

		Device(INTE) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 5)

			Method(_STA, 0) {
				if (PIRE) {
					Return (0x0b) /* sata is invisible */
				} else {
					Return (0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTE._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKE\\_DIS\n") */
				PIRE = 0
			} /* End Method(_SB.INTE._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKE\\_PRS\n") */
				Return (IRQP)
			} /* Method(_SB.INTE._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKE\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				IRQN = 1 << PIRE
				Return (IRQB)
			} /* Method(_SB.INTE._CRS) */

			Method(_SRS, 1) {
				/* DBGO("\\_SB\\LNKE\\_CRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Local0--
				}
				PIRE = Local0
			} /* End Method(_SB.INTE._SRS) */
		} /* End Device(INTE)  */

		Device(INTF) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 6)

			Method(_STA, 0) {
				if (PIRF) {
					Return (0x0b) /* sata is invisible */
				} else {
					Return (0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTF._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKF\\_DIS\n") */
				PIRF = 0
			} /* End Method(_SB.INTF._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKF\\_PRS\n") */
				Return (PITF)
			} /* Method(_SB.INTF._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKF\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				IRQN = 1 << PIRF
				Return (IRQB)
			} /* Method(_SB.INTF._CRS) */

			Method(_SRS, 1) {
				/* DBGO("\\_SB\\LNKF\\_CRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Local0--
				}
				PIRF = Local0
			} /*  End Method(_SB.INTF._SRS) */
		} /* End Device(INTF)  */

		Device(INTG) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 7)

			Method(_STA, 0) {
				if (PIRG) {
					Return (0x0b) /* sata is invisible */
				} else {
					Return (0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTG._STA)  */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKG\\_DIS\n") */
				PIRG = 0
			} /* End Method(_SB.INTG._DIS)  */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKG\\_PRS\n") */
				Return (IRQP)
			} /* Method(_SB.INTG._CRS)  */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKG\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				IRQN = 1 << PIRG
				Return (IRQB)
			} /* Method(_SB.INTG._CRS)  */

			Method(_SRS, 1) {
				/* DBGO("\\_SB\\LNKG\\_CRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Local0--
				}
				PIRG = Local0
			} /* End Method(_SB.INTG._SRS)  */
		} /* End Device(INTG)  */

		Device(INTH) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 8)

			Method(_STA, 0) {
				if (PIRH) {
					Return (0x0b) /* sata is invisible */
				} else {
					Return (0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTH._STA)  */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKH\\_DIS\n") */
				PIRH = 0
			} /* End Method(_SB.INTH._DIS)  */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKH\\_PRS\n") */
				Return (IRQP)
			} /* Method(_SB.INTH._CRS)  */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKH\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				IRQN = 1 << PIRH
				Return (IRQB)
			} /* Method(_SB.INTH._CRS)  */

			Method(_SRS, 1) {
				/* DBGO("\\_SB\\LNKH\\_CRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Local0--
				}
				PIRH = Local0
			} /* End Method(_SB.INTH._SRS)  */
		} /* End Device(INTH)   */

	}   /* End Scope(_SB)  */
