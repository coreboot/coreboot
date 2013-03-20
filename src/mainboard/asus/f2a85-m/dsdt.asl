/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

/* DefinitionBlock Statement */
DefinitionBlock (
	"DSDT.AML",           /* Output filename */
	"DSDT",                 /* Signature */
	0x02,		/* DSDT Revision, needs to be 2 for 64bit */
	"ASUS  ",               /* OEMID */
	"COREBOOT",	     /* TABLE ID */
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	/* #include "../../../arch/x86/acpi/debug.asl" */		/* Include global debug methods if needed */

	/* Data to be patched by the BIOS during POST */
	/* FIXME the patching is not done yet! */
	/* Memory related values */
	Name(LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
	Name(PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
	Name(PBLN, 0x0)	/* Length of BIOS area */

	Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)	/* Base address of PCIe config space */
	Name(PCLN, Multiply(0x100000, CONFIG_MMCONF_BUS_NUMBER)) /* Length of PCIe config space, 1MB each bus */
	Name(HPBA, 0xFED00000)	/* Base address of HPET table */

	Name(SSFG, 0x0D)		/* S1 support: bit 0, S2 Support: bit 1, etc. S0 & S5 assumed */

	/* USB overcurrent mapping pins.   */
	Name(UOM0, 0)
	Name(UOM1, 2)
	Name(UOM2, 0)
	Name(UOM3, 7)
	Name(UOM4, 2)
	Name(UOM5, 2)
	Name(UOM6, 6)
	Name(UOM7, 2)
	Name(UOM8, 6)
	Name(UOM9, 6)

	/* Some global data */
	Name(OSTP, 3)		/* Assume nothing. WinXp = 1, Vista = 2, Linux = 3, WinCE = 4 */
	Name(OSV, Ones)	/* Assume nothing */
	Name(PMOD, One)	/* Assume APIC */

	/*
	 * Processor Object
	 *
	 */
	Scope (\_PR) {		/* define processor scope */
		Processor(
			P000,		/* name space name */
			0,		/* Unique number for this processor */
			0x810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}

		Processor(
			P001,		/* name space name */
			1,		/* Unique number for this processor */
			0x0810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			P002,		/* name space name */
			2,		/* Unique number for this processor */
			0x0810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			P003,		/* name space name */
			3,		/* Unique number for this processor */
			0x0810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			P004,		/* name space name */
			4,		/* Unique number for this processor */
			0x0810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			P005,		/* name space name */
			5,		/* Unique number for this processor */
			0x0810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			P006,		/* name space name */
			6,		/* Unique number for this processor */
			0x0810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			P007,		/* name space name */
			7,		/* Unique number for this processor */
			0x0810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
	} /* End _PR scope */

	/* PIC IRQ mapping registers, C00h-C01h. */
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

	/* Client Management index/data registers */
	OperationRegion(CMT, SystemIO, 0x00000C50, 0x00000002)
		Field(CMT, ByteAcc, NoLock, Preserve) {
		CMTI,      8,
		/* Client Management Data register */
		G64E,   1,
		G64O,      1,
		G32O,      2,
		,       2,
		GPSL,     2,
	}

	/* GPM Port register */
	OperationRegion(GPT, SystemIO, 0x00000C52, 0x00000001)
		Field(GPT, ByteAcc, NoLock, Preserve) {
		GPB0,1,
		GPB1,1,
		GPB2,1,
		GPB3,1,
		GPB4,1,
		GPB5,1,
		GPB6,1,
		GPB7,1,
	}

	/* Flash ROM program enable register */
	OperationRegion(FRE, SystemIO, 0x00000C6F, 0x00000001)
		Field(FRE, ByteAcc, NoLock, Preserve) {
		,     0x00000006,
		FLRE, 0x00000001,
	}

	/* PM2 index/data registers */
	OperationRegion(PM2R, SystemIO, 0x00000CD0, 0x00000002)
		Field(PM2R, ByteAcc, NoLock, Preserve) {
		PM2I, 0x00000008,
		PM2D, 0x00000008,
	}

	/* Power Management I/O registers. */
	OperationRegion(PIOR, SystemIO, 0x00000CD6, 0x00000002)
		Field(PIOR, ByteAcc, NoLock, Preserve) {
		PIOI, 0x00000008,
		PIOD, 0x00000008,
	}
	IndexField (PIOI, PIOD, ByteAcc, NoLock, Preserve) {
		Offset(0xEE),
		UPWS, 3,
	}

	/* PM1 Event Block
	* First word is PM1_Status, Second word is PM1_Enable
	*/
	Scope(\_SB) {
		/* PCIe Configuration Space for CONFIG_MMCONF_BUS_NUMBER busses */
		OperationRegion(PCFG, SystemMemory, PCBA, PCLN)
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
	}

	#include "acpi/routing.asl"

	Scope(\_SB) {

		Method(CkOT, 0){

			if(LNotEqual(OSTP, Ones)) {Return(OSTP)}	/* OS version was already detected */

			if(CondRefOf(\_OSI,Local1))
			{
				Store(1, OSTP)                /* Assume some form of XP */
				if (\_OSI("Windows 2006"))      /* Vista */
				{
					Store(2, OSTP)
				}
			} else {
				If(WCMP(\_OS,"Linux")) {
					Store(3, OSTP)            /* Linux */
				} Else {
					Store(4, OSTP)            /* Gotta be WinCE */
				}
			}
			Return(OSTP)
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
			//Store(0, PIRA)
			//Store(0, PIRB)
			//Store(0, PIRC)
			//Store(0, PIRD)
			//Store(0, PIRE)
			//Store(0, PIRF)
			//Store(0, PIRG)
			//Store(0, PIRH)
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
				//Store(0x1F, PIRA)
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
				/* DBGO("\\_SB\\LNKA\\_CRS\n") */
				CreateWordField(ARG0, 1, IRQM)

				/* Use lowest available IRQ */
				FindSetRightBit(IRQM, Local0)
				if (Local0) {
					Decrement(Local0)
				}
				//Store(Local0, PIRA)
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
				//Store(0, PIRB)
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
				//Store(0, PIRC)
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
				//Store(0, PIRD)
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
				//Store(0, PIRE)
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
				//Store(0, PIRF)
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
				//Store(0, PIRG)
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
				//Store(0, PIRH)
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
				//Store(Local0, PIRH)
			} /* End Method(_SB.INTH._SRS)  */
		} /* End Device(INTH)   */

	}   /* End Scope(_SB)  */

	/* Supported sleep states: */
	Name(\_S0, Package () {0x00, 0x00, 0x00, 0x00} )	/* (S0) - working state */

	If (LAnd(SSFG, 0x01)) {
		Name(\_S1, Package () {0x01, 0x01, 0x00, 0x00} )	/* (S1) - sleeping w/CPU context */
	}
	If (LAnd(SSFG, 0x02)) {
		Name(\_S2, Package () {0x02, 0x02, 0x00, 0x00} )	/* (S2) - "light" Suspend to RAM */
	}
	If (LAnd(SSFG, 0x04)) {
		Name(\_S3, Package () {0x03, 0x03, 0x00, 0x00} )	/* (S3) - Suspend to RAM */
	}
	If (LAnd(SSFG, 0x08)) {
		Name(\_S4, Package () {0x04, 0x04, 0x00, 0x00} )	/* (S4) - Suspend to Disk */
	}

	Name(\_S5, Package () {0x05, 0x05, 0x00, 0x00} )	/* (S5) - Soft Off */

	Name(\_SB.CSPS ,0)				/* Current Sleep State (S0, S1, S2, S3, S4, S5) */
	Name(CSMS, 0)			/* Current System State */

	/* Wake status package */
	Name(WKST,Package(){Zero, Zero})

	/*
	* \_PTS - Prepare to Sleep method
	*
	*	Entry:
	*		Arg0=The value of the sleeping state S1=1, S2=2, etc
	*
	* Exit:
	*		-none-
	*
	* The _PTS control method is executed at the beginning of the sleep process
	* for S1-S5. The sleeping value is passed to the _PTS control method.	This
	* control method may be executed a relatively long time before entering the
	* sleep state and the OS may abort	the operation without notification to
	* the ACPI driver.  This method cannot modify the configuration or power
	* state of any device in the system.
	*/
	Method(_PTS, 1) {
		/* DBGO("\\_PTS\n") */
		/* DBGO("From S0 to S") */
		/* DBGO(Arg0) */
		/* DBGO("\n") */

		/* Don't allow PCIRST# to reset USB */
		//if (LEqual(Arg0,3)){
		//	Store(0,URRE)
		//}

		/* Clear sleep SMI status flag and enable sleep SMI trap. */
		/*Store(One, CSSM)
		Store(One, SSEN)*/

		/* On older chips, clear PciExpWakeDisEn */
		/*if (LLessEqual(\_SB.SBRI, 0x13)) {
		*    	Store(0,\_SB.PWDE)
		*}
		*/

		/* Clear wake status structure. */
		Store(0, Index(WKST,0))
		Store(0, Index(WKST,1))
		Store(7, UPWS)
	} /* End Method(\_PTS) */

	/*
	*  The following method results in a "not a valid reserved NameSeg"
	*  warning so I have commented it out for the duration.  It isn't
	*  used, so it could be removed.
	*
	*
	*  	\_GTS OEM Going To Sleep method
	*
	*  	Entry:
	*  		Arg0=The value of the sleeping state S1=1, S2=2
	*
	*  	Exit:
	*  		-none-
	*
	*  Method(\_GTS, 1) {
	*  DBGO("\\_GTS\n")
	*  DBGO("From S0 to S")
	*  DBGO(Arg0)
	*  DBGO("\n")
	*  }
	*/

	/*
	*	\_BFS OEM Back From Sleep method
	*
	*	Entry:
	*		Arg0=The value of the sleeping state S1=1, S2=2
	*
	*	Exit:
	*		-none-
	*/
	Method(\_BFS, 1) {
		/* DBGO("\\_BFS\n") */
		/* DBGO("From S") */
		/* DBGO(Arg0) */
		/* DBGO(" to S0\n") */
	}

	/*
	*  \_WAK System Wake method
	*
	*	Entry:
	*		Arg0=The value of the sleeping state S1=1, S2=2
	*
	*	Exit:
	*		Return package of 2 DWords
	*		Dword 1 - Status
	*			0x00000000	wake succeeded
	*			0x00000001	Wake was signaled but failed due to lack of power
	*			0x00000002	Wake was signaled but failed due to thermal condition
	*		Dword 2 - Power Supply state
	*			if non-zero the effective S-state the power supply entered
	*/
	Method(\_WAK, 1) {
		/* DBGO("\\_WAK\n") */
		/* DBGO("From S") */
		/* DBGO(Arg0) */
		/* DBGO(" to S0\n") */

		/* Re-enable HPET */
		//Store(1,HPDE)

		/* Restore PCIRST# so it resets USB */
		//if (LEqual(Arg0,3)){
		//	Store(1,URRE)
		//}

		/* Arbitrarily clear PciExpWakeStatus */
		//Store(PWST, PWST)

		/* if(DeRefOf(Index(WKST,0))) {
		*	Store(0, Index(WKST,1))
		* } else {
		*	Store(Arg0, Index(WKST,1))
		* }
		*/
		Return(WKST)
	} /* End Method(\_WAK) */

	Scope(\_GPE) {	/* Start Scope GPE */
		/*  General event 0  */
		/* Method(_L00) {
		*	DBGO("\\_GPE\\_L00\n")
		* }
		*/

		/*  General event 1  */
		/* Method(_L01) {
		*	DBGO("\\_GPE\\_L00\n")
		* }
		*/

		/*  General event 2  */
		/* Method(_L02) {
		*	DBGO("\\_GPE\\_L00\n")
		* }
		*/

		/*  General event 3  */
		Method(_L03) {
			/* DBGO("\\_GPE\\_L00\n") */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  General event 4  */
		/* Method(_L04) {
		*	DBGO("\\_GPE\\_L00\n")
		* }
		*/

		/*  General event 5  */
		/* Method(_L05) {
		*	DBGO("\\_GPE\\_L00\n")
		* }
		*/

		/*  General event 6 - Used for GPM6, moved to USB.asl */
		/* Method(_L06) {
		*	DBGO("\\_GPE\\_L00\n")
		* }
		*/

		/*  General event 7 - Used for GPM7, moved to USB.asl */
		/* Method(_L07) {
		*	DBGO("\\_GPE\\_L07\n")
		* }
		*/

		/*  Legacy PM event  */
		Method(_L08) {
			/* DBGO("\\_GPE\\_L08\n") */
		}

		/*  Temp warning (TWarn) event  */
		Method(_L09) {
			/* DBGO("\\_GPE\\_L09\n") */
			/* Notify (\_TZ.TZ00, 0x80) */
		}

		/*  Reserved  */
		/* Method(_L0A) {
		*	DBGO("\\_GPE\\_L0A\n")
		* }
		*/

		/*  USB controller PME#  */
		Method(_L0B) {
			/* DBGO("\\_GPE\\_L0B\n") */
			Notify(\_SB.PCI0.UOH1, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH2, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH3, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH4, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH5, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH6, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UEH1, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  AC97 controller PME#  */
		/* Method(_L0C) {
		*	DBGO("\\_GPE\\_L0C\n")
		* }
		*/

		/*  OtherTherm PME#  */
		/* Method(_L0D) {
		*	DBGO("\\_GPE\\_L0D\n")
		* }
		*/

		/*  GPM9 SCI event - Moved to USB.asl */
		/* Method(_L0E) {
		*	DBGO("\\_GPE\\_L0E\n")
		* }
		*/

		/*  PCIe HotPlug event  */
		/* Method(_L0F) {
		* 	DBGO("\\_GPE\\_L0F\n")
		* }
		*/

		/*  ExtEvent0 SCI event  */
		Method(_L10) {
			/* DBGO("\\_GPE\\_L10\n") */
		}

		/*  ExtEvent1 SCI event  */
		Method(_L11) {
			/* DBGO("\\_GPE\\_L11\n") */
		}

		/*  PCIe PME# event  */
		/* Method(_L12) {
		*	DBGO("\\_GPE\\_L12\n")
		* }
		*/

		/*  GPM0 SCI event - Moved to USB.asl */
		/* Method(_L13) {
		* 	DBGO("\\_GPE\\_L13\n")
		* }
		*/

		/*  GPM1 SCI event - Moved to USB.asl */
		/* Method(_L14) {
		* 	DBGO("\\_GPE\\_L14\n")
		* }
		*/

		/*  GPM2 SCI event - Moved to USB.asl */
		/* Method(_L15) {
		* 	DBGO("\\_GPE\\_L15\n")
		* }
		*/

		/*  GPM3 SCI event - Moved to USB.asl */
		/* Method(_L16) {
		*	DBGO("\\_GPE\\_L16\n")
		* }
		*/

		/*  GPM8 SCI event - Moved to USB.asl */
		/* Method(_L17) {
		* 	DBGO("\\_GPE\\_L17\n")
		* }
		*/

		/*  GPIO0 or GEvent8 event  */
		Method(_L18) {
			/* DBGO("\\_GPE\\_L18\n") */
			Notify(\_SB.PCI0.PBR4, 0x02) /* NOTIFY_DEVICE_WAKE */
#if 0
			Notify(\_SB.PCI0.PBR5, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR6, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR7, 0x02) /* NOTIFY_DEVICE_WAKE */
#endif
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  GPM4 SCI event - Moved to USB.asl */
		/* Method(_L19) {
		* 	DBGO("\\_GPE\\_L19\n")
		* }
		*/

		/*  GPM5 SCI event - Moved to USB.asl */
		/* Method(_L1A) {
		*	DBGO("\\_GPE\\_L1A\n")
		* }
		*/

		/*  Azalia SCI event  */
		Method(_L1B) {
			/* DBGO("\\_GPE\\_L1B\n") */
			Notify(\_SB.PCI0.AZHD, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  GPM6 SCI event - Reassigned to _L06 */
		/* Method(_L1C) {
		*	DBGO("\\_GPE\\_L1C\n")
		* }
		*/

		/*  GPM7 SCI event - Reassigned to _L07 */
		/* Method(_L1D) {
		*	DBGO("\\_GPE\\_L1D\n")
		* }
		*/

		/*  GPIO2 or GPIO66 SCI event  */
		/* Method(_L1E) {
		* 	DBGO("\\_GPE\\_L1E\n")
		* }
		*/

		/*  SATA SCI event - Moved to sata.asl */
		/* Method(_L1F) {
		*	 DBGO("\\_GPE\\_L1F\n")
		* }
		*/

	} 	/* End Scope GPE */

	//#include "acpi/usb.asl"

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		#include "../../../arch/x86/acpi/globutil.asl" /* global utility methods expected within the \_SB scope */

		/*  _SB.PCI0 */
		/* Note: Only need HID on Primary Bus */
		Device(PCI0) {
			External (TOM1)
			External (TOM2)
			Name(_HID, EISAID("PNP0A03"))
			Name(_ADR, 0x00180000)	/* Dev# = BSP Dev#, Func# = 0 */
			Method(_BBN, 0) { /* Bus number = 0 */
				Return(0)
			}
			Method(_STA, 0) {
				/* DBGO("\\_SB\\PCI0\\_STA\n") */
				Return(0x0B)     /* Status is visible */
			}

			Method(_PRT,0) {
				If(PMOD){ Return(APR0) }   /* APIC mode */
				Return (PR0)                  /* PIC Mode */
			} /* end _PRT */

			/* Describe the Northbridge devices Dev0 ,Func0*/
			Device(AMRT) {
				Name(_ADR, 0x00000000)
			} /* end AMRT */

			/* Dev2 is also an external GFX bridge */
			Device(PBR2) {
				Name(_ADR, 0x00020000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PMOD){ Return(APS2) }   /* APIC mode */
					Return (PS2)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR2 */

			Device(PBR4) {
				Name(_ADR, 0x00040000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PMOD){ Return(APS4) }   /* APIC mode */
					Return (PS4)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR4 */

			/* PCI slot 1, 2, 3 */
			Device(PIBR) {
				Name(_ADR, 0x00140004)
				Name(_PRW, Package() {0x18, 4})
				/* TODO: non APIC rounting */
				Method(_PRT,0) {
					Return (PCIB)
				}
			}

			/* Describe the Southbridge devices */
			/* PCIe 1x SB */
			Device(SBR0) {
				Name(_ADR, 0x00150000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PMOD){ Return(ABR0) }   /* APIC mode */
					Return (PBR0)                  /* PIC Mode */
				} /* end _PRT */
			} /* end SBR0 */


			/* Onboard network */
			Device(SBR1) {
				Name(_ADR, 0x00150001)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT, 0) {
					If(PMOD){ Return(ABR1) }   /* APIC mode */
					Return (PBR1)                  /* PIC Mode */
				} /* end _PRT */
			} /* end SBR1 */

			Device(STCR) {
				Name(_ADR, 0x00110000)
				//#include "acpi/sata.asl"
			} /* end STCR */

			Device(UOH1) {
				Name(_ADR, 0x00120000)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH1 */

			Device(UOH2) {
				Name(_ADR, 0x00120002)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH2 */

			Device(UOH3) {
				Name(_ADR, 0x00130000)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH3 */

			Device(UOH4) {
				Name(_ADR, 0x00130002)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH4 */

			Device(UOH5) {
				Name(_ADR, 0x00160000)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH5 */

			Device(UOH6) {
				Name(_ADR, 0x00160002)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH5 */

			Device(UEH1) {
				Name(_ADR, 0x00140005)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UEH1 */

			Device(XHC0) {
				Name(_ADR, 0x00100000)
				Name(_PRW, Package() {0x0B, 4})
			} /* end XHC0 */
			Device(XHC1) {
				Name(_ADR, 0x00100001)
				Name(_PRW, Package() {0x0B, 4})
			} /* end XHC1 */

			Device(SBUS) {
				Name(_ADR, 0x00140000)
			} /* end SBUS */

			/* Primary (and only) IDE channel */
			Device(IDEC) {
				Name(_ADR, 0x00140001)
				//#include "acpi/ide.asl"
			} /* end IDEC */

			Device(AZHD) {
				Name(_ADR, 0x00140002)
				OperationRegion(AZPD, PCI_Config, 0x00, 0x100)
					Field(AZPD, AnyAcc, NoLock, Preserve) {
					offset (0x42),
					NSDI, 1,
					NSDO, 1,
					NSEN, 1,
					offset (0x44),
					IPCR, 4,
					offset (0x54),
					PWST, 2,
					, 6,
					PMEB, 1,
					, 6,
					PMST, 1,
					offset (0x62),
					MMCR, 1,
					offset (0x64),
					MMLA, 32,
					offset (0x68),
					MMHA, 32,
					offset (0x6C),
					MMDT, 16,
				}

				Method(_INI) {
					If(LEqual(OSTP,3)){   /* If we are running Linux */
						Store(zero, NSEN)
						Store(one, NSDO)
						Store(one, NSDI)
					}
				}
			} /* end AZHD */

			Device(LIBR) {
				Name(_ADR, 0x00140003)
				/* Method(_INI) {
				*	DBGO("\\_SB\\PCI0\\LpcIsaBr\\_INI\n")
				} */ /* End Method(_SB.SBRDG._INI) */

				/* Real Time Clock Device */
				Device(RTC0) {
					Name(_HID, EISAID("PNP0B00"))   /* AT Real Time Clock (not PIIX4 compatible) */
					Name(_CRS, ResourceTemplate() {
						IRQNoFlags(){8}
						IO(Decode16,0x0070, 0x0070, 0, 2)
						/* IO(Decode16,0x0070, 0x0070, 0, 4) */
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.RTC0) */

				Device(TMR) {	/* Timer */
					Name(_HID,EISAID("PNP0100"))	/* System Timer */
					Name(_CRS, ResourceTemplate() {
						IRQNoFlags(){0}
						IO(Decode16, 0x0040, 0x0040, 0, 4)
						/* IO(Decode16, 0x0048, 0x0048, 0, 4) */
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.TMR) */

				Device(SPKR) {	/* Speaker */
					Name(_HID,EISAID("PNP0800"))	/* AT style speaker */
					Name(_CRS, ResourceTemplate() {
						IO(Decode16, 0x0061, 0x0061, 0, 1)
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.SPKR) */

				Device(PIC) {
					Name(_HID,EISAID("PNP0000"))	/* AT Interrupt Controller */
					Name(_CRS, ResourceTemplate() {
						IRQNoFlags(){2}
						IO(Decode16,0x0020, 0x0020, 0, 2)
						IO(Decode16,0x00A0, 0x00A0, 0, 2)
						/* IO(Decode16, 0x00D0, 0x00D0, 0x10, 0x02) */
						/* IO(Decode16, 0x04D0, 0x04D0, 0x10, 0x02) */
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.PIC) */

				Device(MAD) { /* 8257 DMA */
					Name(_HID,EISAID("PNP0200"))	/* Hardware Device ID */
					Name(_CRS, ResourceTemplate() {
						DMA(Compatibility,BusMaster,Transfer8){4}
						IO(Decode16, 0x0000, 0x0000, 0x10, 0x10)
						IO(Decode16, 0x0081, 0x0081, 0x01, 0x03)
						IO(Decode16, 0x0087, 0x0087, 0x01, 0x01)
						IO(Decode16, 0x0089, 0x0089, 0x01, 0x03)
						IO(Decode16, 0x008F, 0x008F, 0x01, 0x01)
						IO(Decode16, 0x00C0, 0x00C0, 0x10, 0x20)
					}) /* End Name(_SB.PCI0.LpcIsaBr.MAD._CRS) */
				} /* End Device(_SB.PCI0.LpcIsaBr.MAD) */

				Device(COPR) {
					Name(_HID,EISAID("PNP0C04"))	/* Math Coprocessor */
					Name(_CRS, ResourceTemplate() {
						IO(Decode16, 0x00F0, 0x00F0, 0, 0x10)
						IRQNoFlags(){13}
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.COPR) */
			} /* end LIBR */

			Device(HPBR) {
				Name(_ADR, 0x00140004)
			} /* end HostPciBr */

			Device(ACAD) {
				Name(_ADR, 0x00140005)
			} /* end Ac97audio */

			Device(ACMD) {
				Name(_ADR, 0x00140006)
			} /* end Ac97modem */

			Name(CRES, ResourceTemplate() {
				IO(Decode16, 0x0CF8, 0x0CF8, 1,	8)

				WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,			/* address granularity */
					0x0000,			/* range minimum */
					0x0CF7,			/* range maximum */
					0x0000,			/* translation */
					0x0CF8			/* length */
				)
				WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,			/* address granularity */
					0x03B0,			/* range minimum */
					0x03DF,			/* range maximum */
					0x0000,			/* translation */
					0x0030			/* length */
				)

				WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,			/* address granularity */
					0x0D00,			/* range minimum */
					0xFFFF,			/* range maximum */
					0x0000,			/* translation */
					0xF300			/* length */
				)

				Memory32Fixed(READONLY, 0x000A0000, 0x00020000, VGAM) 	/* VGA memory space */
				Memory32Fixed(READONLY, 0x000C0000, 0x00020000, EMM1)	/* Assume C0000-E0000 empty */
                                /* memory space for PCI BARs below 4GB */
                                Memory32Fixed(ReadOnly, 0x00000000, 0x00000000, MMIO)
			}) /* End Name(_SB.PCI0.CRES) */

			Method(_CRS, 0) {
				/* DBGO("\\_SB\\PCI0\\_CRS\n") */
				CreateDWordField(CRES, ^MMIO._BAS, MM1B)
                                CreateDWordField(CRES, ^MMIO._LEN, MM1L)
                                /*
                                 * Declare memory between TOM1 and 4GB as available
                                 * for PCI MMIO.
                                 * Use ShiftLeft to avoid 64bit constant (for XP).
                                 * This will work even if the OS does 32bit arithmetic, as
                                 * 32bit (0x00000000 - TOM1) will wrap and give the same
                                 * result as 64bit (0x100000000 - TOM1).
                                 */
                                Store(TOM1, MM1B)
                                ShiftLeft(0x10000000, 4, Local0)
                                Subtract(Local0, TOM1, Local0)
                                Store(Local0, MM1L)

				Return(CRES) /* note to change the Name buffer */
			} /* end of Method(_SB.PCI0._CRS) */

			/*
			*
			*               FIRST METHOD CALLED UPON BOOT
			*
			*  1. If debugging, print current OS and ACPI interpreter.
			*  2. Get PCI Interrupt routing from ACPI VSM, this
			*     value is based on user choice in BIOS setup.
			*/
			Method(_INI, 0) {
				/* DBGO("\\_SB\\_INI\n") */
				/* DBGO("   DSDT.ASL code from ") */
				/* DBGO(__DATE__) */
				/* DBGO(" ") */
				/* DBGO(__TIME__) */
				/* DBGO("\n   Sleep states supported: ") */
				/* DBGO("\n") */
				/* DBGO("   \\_OS=") */
				/* DBGO(\_OS) */
				/* DBGO("\n   \\_REV=") */
				/* DBGO(\_REV) */
				/* DBGO("\n") */

				/* Determine the OS we're running on */
				CkOT()

				/* On older chips, clear PciExpWakeDisEn */
				/*if (LLessEqual(\SBRI, 0x13)) {
				*    	Store(0,\PWDE)
				* }
				*/
			} /* End Method(_SB._INI) */
		} /* End Device(PCI0)  */

		Device(PWRB) {	/* Start Power button device */
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_PRW, Package () {3, 0x04})	/* wake from S1-S4 */
			Name(_STA, 0x0B) /* sata is invisible */
		}
	} /* End \_SB scope */

	Scope(\_SI) {
		Method(_SST, 1) {
			/* DBGO("\\_SI\\_SST\n") */
			/* DBGO("   New Indicator state: ") */
			/* DBGO(Arg0) */
			/* DBGO("\n") */
		}
	} /* End Scope SI */
}
/* End of ASL file */
