/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

/* DefinitionBlock Statement */
DefinitionBlock (
	"DSDT.AML",           /* Output filename */
	"DSDT",                 /* Signature */
	0x02,		/* DSDT Revision, needs to be 2 for 64bit */
	"AMD   ",               /* OEMID */
	"COREBOOT",	     /* TABLE ID */
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	/* #include <arch/x86/acpi/debug.asl> */	/* Include global debug methods if needed */

	/* Data to be patched by the BIOS during POST */
	/* FIXME the patching is not done yet! */
	/* Memory related values */
	Name(LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
	Name(PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
	Name(PBLN, 0x0)	/* Length of BIOS area */

	Name(PCBA, 0xE0000000)	/* Base address of PCIe config space */
	Name(HPBA, 0xFED00000)	/* Base address of HPET table */

	/* Some global data */
	Name(OSV, Ones)	/* Assume nothing */
	Name(GPIC, 0x1)	/* Assume PIC */

	/*
	 * Processor Object
	 *
	 */
	Scope (\_PR) {		/* define processor scope */
		Processor(
			C000,		/* name space name, align with BLDCFG_PROCESSOR_SCOPE_NAME[01] */
			0,		/* Unique number for this processor */
			0x810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			C001,		/* name space name */
			1,		/* Unique number for this processor */
			0x810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			C002,		/* name space name */
			2,		/* Unique number for this processor */
			0x810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(
			C003,		/* name space name */
			3,		/* Unique number for this processor */
			0x810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
	} /* End _PR scope */

	/* PIC IRQ mapping registers, C00h-C01h. */
	OperationRegion(PIRQ, SystemIO, 0x00000C00, 0x00000002)
		Field(PIRQ, ByteAcc, NoLock, Preserve) {
		PIDX, 0x00000008,
		PDAT, 0x00000008,  /* Offset: 1h */
	}
	IndexField(PIDX, PDAT, ByteAcc, NoLock, Preserve) {
		PIRA, 0x00000008,	/* Index 0 */
		PIRB, 0x00000008,	/* Index 1 */
		PIRC, 0x00000008,	/* Index 2 */
		PIRD, 0x00000008,	/* Index 3 */
		PIRE, 0x00000008,	/* Index 4 */
		PIRF, 0x00000008,	/* Index 5 */
		PIRG, 0x00000008,	/* Index 6 */
		PIRH, 0x00000008,	/* Index 7 */
		Offset(0x10),
		PIRS, 0x00000008,
		Offset(0x13),
		HDAD, 0x00000008,
		, 0x00000008,
		GEC,  0x00000008,
		Offset(0x30),
		USB1, 0x00000008,
		USB2, 0x00000008,
		USB3, 0x00000008,
		USB4, 0x00000008,
		USB5, 0x00000008,
		USB6, 0x00000008,
		USB7, 0x00000008,
		Offset(0x40),
		IDE,  0x00000008,
		SATA, 0x00000008,
		Offset(0x50),
		GPP0, 0x00000008,
		GPP1, 0x00000008,
		GPP2, 0x00000008,
		GPP3, 0x00000008
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

	/* Power Management I/O registers, TODO:PMIO is quite different in SB900. */
	OperationRegion(PMRG, SystemIO, 0x00000CD6, 0x00000002)
		Field(PMRG, ByteAcc, NoLock, Preserve) {
		PMRI, 0x00000008,
		PMRD, 0x00000008,
	}
	IndexField (PMRI, PMRD, ByteAcc, NoLock, Preserve) {
		Offset(0x24),
		MMSO,32,
		Offset(0x37),	/* GPMLevelConfig0 */
		, 3,
		PLC0, 1,
		PLC1, 1,
		PLC2, 1,
		PLC3, 1,
		PLC8, 1,
		Offset(0x38),	/* GPMLevelConfig1 */
		, 1,
		 PLC4, 1,
		 PLC5, 1,
		, 1,
		 PLC6, 1,
		 PLC7, 1,
		Offset(0x50),
		HPAD,32,
		Offset(0x60),
		P1EB,16,
		Offset(0x65),	/* UsbPMControl */
		, 4,
		URRE, 1,
		Offset(0x96),	/* GPM98IN */
		G8IS, 1,
		G9IS, 1,
		Offset(0x9A),	/* EnhanceControl */
		,7,
		HPDE, 1,
		Offset(0xC8),
		,2,
		SPRE,1,
		TPDE,1,
		Offset(0xF0),
		,3,
		RSTU,1
	}

	/* PM1 Event Block
	* First word is PM1_Status, Second word is PM1_Enable
	*/
	OperationRegion(P1E0, SystemIO, P1EB, 0x04)
		Field(P1E0, ByteAcc, NoLock, Preserve) {
		,14,
		PEWS,1,
		WSTA,1,
		,14,
		PEWD,1
	}

	OperationRegion (GRAM, SystemMemory, 0x0400, 0x0100)
	Field (GRAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x10),
		FLG0,   8
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
	}


	#include "acpi/routing.asl"

	Scope(\_SB) {

		/* Debug Port registers, 80h. */
		OperationRegion(DBBG, SystemIO, 0x00000080, 0x00000001)
			Field(DBBG, ByteAcc, NoLock, Preserve) {
			DBG8, 0x00000008,
		}

		Method(_PIC, 1) {
			Store(Arg0, GPIC)
			If (GPIC) {
				Store(0xAA, \_SB.DBG8)
				\_SB.DSPI()
			} else {
				Store(0xAC, \_SB.DBG8)
			}
		}

		Method(DSPI, 0) {
			\_SB.GRUA(0x1F)
			\_SB.GRUB(0x1F)
			\_SB.GRUC(0x1F)
			\_SB.GRUD(0x1F)
			Store(0x1F, PIRE)
			Store(0x1F, PIRF)
			Store(0x1F, PIRG)
			Store(0x1F, PIRH)
		}

		Method(GRUA, 1) {
			Store(Arg0, PIRA)
			Store(Arg0, HDAD)
			Store(Arg0, GEC)
			Store(Arg0, GPP0)
			Store(Arg0, GPP0)
		}

		Method(GRUB, 1)	{
			Store(Arg0, PIRB)
			Store(Arg0, USB2)
			Store(Arg0, USB4)
			Store(Arg0, USB6)
			Store(Arg0, GPP1)
			Store(Arg0, IDE)
		}

		Method(GRUC, 1)	{
			Store(Arg0, PIRC)
			Store(Arg0, USB1)
			Store(Arg0, USB3)
			Store(Arg0, USB5)
			Store(Arg0, USB7)
			Store(Arg0, GPP2)
		}

		Method(GRUD, 1)	{
			Store(Arg0, PIRD)
			Store(Arg0, SATA)
			Store(Arg0, GPP3)
		}

		Name(IRQB, ResourceTemplate() {
			IRQ(Level, ActiveLow, Shared) {
				15
		}})

		Name(IRQP, ResourceTemplate() {
			IRQ(Level, ActiveLow, Shared) {
				3, 4, 5, 7, 10, 11, 12, 14, 15
		}})

		Device(INTA) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 1)
			Method(_STA, 0) {
				if (PIRA) {
					Return(0x0B)
				} else {
					Return(0x09)
				}
			}
			Method(_DIS ,0) {
				\_SB.GRUA(0x1F)
			}
			Method(_PRS ,0) {
				Return(IRQP)
			}
			Method(_CRS ,0) {
				CreateWordField(IRQB, 1, IRQN)
				ShiftLeft(1, PIRA, IRQN)
				Return(IRQB)
			}
			Method(_SRS, 1) {
				CreateWordField(Arg0, 1, IRQM)
				FindSetRightBit(IRQM, Local0)
				Decrement(Local0)
				\_SB.GRUA(Local0)
			}
		}

		Device(INTB) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 2)
			Method(_STA, 0) {
				if (PIRB) {
					Return(0x0B)
				} else {
					Return(0x09)
				}
			}
			Method(_DIS ,0) {
				\_SB.GRUB(0x1F)
			}
			Method(_PRS ,0) {
				Return(IRQP)
			}
			Method(_CRS ,0) {
				CreateWordField(IRQB, 1, IRQN)
				ShiftLeft(1, PIRB, IRQN)
				Return(IRQB)
			}
			Method(_SRS, 1) {
				CreateWordField(Arg0, 1, IRQM)
				FindSetRightBit(IRQM, Local0)
				Decrement(Local0)
				\_SB.GRUB(Local0)
			}
		}

		Device(INTC) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 3)
			Method(_STA, 0) {
				if (PIRC) {
					Return(0x0B)
				} else {
					Return(0x09)
				}
			}
			Method(_DIS ,0) {
				\_SB.GRUC(0x1F)
			}
			Method(_PRS ,0) {
				Return(IRQP)
			}
			Method(_CRS ,0) {
				CreateWordField(IRQB, 1, IRQN)
				ShiftLeft(1, PIRC, IRQN)
				Return(IRQB)
			}
			Method(_SRS, 1) {
				CreateWordField(Arg0, 1, IRQM)
				FindSetRightBit(IRQM, Local0)
				Decrement(Local0)
				\_SB.GRUC(Local0)
			}
		}

		Device(INTD) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 4)
			Method(_STA, 0) {
				if (PIRD) {
					Return(0x0B)
				} else {
					Return(0x09)
				}
			}
			Method(_DIS ,0) {
				\_SB.GRUD(0x1F)
			}
			Method(_PRS ,0) {
				Return(IRQP)
			}
			Method(_CRS ,0) {
				CreateWordField(IRQB, 1, IRQN)
				ShiftLeft(1, PIRD, IRQN)
				Return(IRQB)
			}
			Method(_SRS, 1) {
				CreateWordField(Arg0, 1, IRQM)
				FindSetRightBit(IRQM, Local0)
				Decrement(Local0)
				\_SB.GRUD(Local0)
			}
		}

		Device(INTE) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 5)
			Method(_STA, 0) {
				if (PIRE) {
					Return(0x0B)
				} else {
					Return(0x09)
				}
			}
			Method(_DIS ,0) {
				Store(0x1F, PIRE)
			}
			Method(_PRS ,0) {
				Return(IRQP)
			}
			Method(_CRS ,0) {
				CreateWordField(IRQB, 1, IRQN)
				ShiftLeft(1, PIRE, IRQN)
				Return(IRQB)
			}
			Method(_SRS, 1) {
				CreateWordField(Arg0, 1, IRQM)
				FindSetRightBit(IRQM, Local0)
				Decrement(Local0)
				Store(Local0, PIRE)
			}
		}

		Device(INTF) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 6)
			Method(_STA, 0) {
				if (PIRF) {
					Return(0x0B)
				} else {
					Return(0x09)
				}
			}
			Method(_DIS ,0) {
				Store(0x1F, PIRF)
			}
			Method(_PRS ,0) {
				Return(IRQP)
			}
			Method(_CRS ,0) {
				CreateWordField(IRQB, 1, IRQN)
				ShiftLeft(1, PIRF, IRQN)
				Return(IRQB)
			}
			Method(_SRS, 1) {
				CreateWordField(Arg0, 1, IRQM)
				FindSetRightBit(IRQM, Local0)
				Decrement(Local0)
				Store(Local0, PIRF)
			}
		}

		Device(INTG) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 7)
			Method(_STA, 0) {
				if (PIRG) {
					Return(0x0B)
				} else {
					Return(0x09)
				}
			}
			Method(_DIS ,0) {
				Store(0x1F, PIRG)
			}
			Method(_PRS ,0) {
				Return(IRQP)
			}
			Method(_CRS ,0) {
				CreateWordField(IRQB, 1, IRQN)
				ShiftLeft(1, PIRG, IRQN)
				Return(IRQB)
			}
			Method(_SRS, 1) {
				CreateWordField(Arg0, 1, IRQM)
				FindSetRightBit(IRQM, Local0)
				Decrement(Local0)
				Store(Local0, PIRG)
			}
		}

		Device(INTH) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 8)
			Method(_STA, 0) {
				if (PIRH) {
					Return(0x0B)
				} else {
					Return(0x09)
				}
			}
			Method(_DIS ,0) {
				Store(0x1F, PIRH)
			}
			Method(_PRS ,0) {
				Return(IRQP)
			}
			Method(_CRS ,0) {
				CreateWordField(IRQB, 1, IRQN)
				ShiftLeft(1, PIRH, IRQN)
				Return(IRQB)
			}
			Method(_SRS, 1) {
				CreateWordField(Arg0, 1, IRQM)
				FindSetRightBit(IRQM, Local0)
				Decrement(Local0)
				Store(Local0, PIRH)
			}
		}
	}   /* End Scope(_SB)  */

	/* Contains the supported sleep states for this chipset */
	#include <southbridge/amd/common/acpi/sleepstates.asl>

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
	* for S1-S5. The sleeping value is passed to the _PTS control method.  This
	* control method may be executed a relatively long time before entering the
	* sleep state and the OS may abort the operation without notification to
	* the ACPI driver.  This method cannot modify the configuration or power
	* state of any device in the system.
	*/
	Method(\_PTS, 1) {
		/* DBGO("\\_PTS\n") */
		/* DBGO("From S0 to S") */
		/* DBGO(Arg0) */
		/* DBGO("\n") */

		/* Don't allow PCIRST# to reset USB */
		if (LEqual(Arg0,3)){
			Store(0,URRE)
		}

		/* Clear sleep SMI status flag and enable sleep SMI trap. */
		/*Store(One, CSSM)
		Store(One, SSEN)*/

		/* On older chips, clear PciExpWakeDisEn */
		/*if (LLessEqual(\_SB.SBRI, 0x13)) {
		*	Store(0,\_SB.PWDE)
		*}
		*/

		/* Clear wake status structure. */
		Store(0, Index(WKST,0))
		Store(0, Index(WKST,1))
	} /* End Method(\_PTS) */

	/*
	*  The following method results in a "not a valid reserved NameSeg"
	*  warning so I have commented it out for the duration.  It isn't
	*  used, so it could be removed.
	*
	*
	*	\_GTS OEM Going To Sleep method
	*
	*	Entry:
	*		Arg0=The value of the sleeping state S1=1, S2=2
	*
	*	Exit:
	*		-none-
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
		Store(1,HPDE)

		/* Restore PCIRST# so it resets USB */
		if (LEqual(Arg0,3)){
			Store(1,URRE)
		}

		/* Arbitrarily clear PciExpWakeStatus */
		Store(PEWS, Local0)
		Store(Local0, PEWS)

		/* if (DeRefOf(Index(WKST,0))) {
		*	Store(0, Index(WKST,1))
		* } else {
		*	Store(Arg0, Index(WKST,1))
		* }
		*/
		Return(WKST)
	} /* End Method(\_WAK) */

	Scope(\_GPE) {	/* Start Scope GPE */
	}	/* End Scope GPE */

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		#include <arch/x86/acpi/globutil.asl> /* global utility methods expected within the \_SB scope */

		/* Primary (and only) IDE channel */
		Device(IDEC) {
			Name(_ADR, 0x00140001)
			#include "acpi/ide.asl"
		} /* end IDEC */

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
				If(GPIC){ Return(APR0) }   /* APIC mode */
				Return (PR0)                  /* PIC Mode */
			} /* end _PRT */

			/* Describe the Northbridge devices */
			Device(AMRT) {
				Name(_ADR, 0x00000000)
			} /* end AMRT */

			/* The internal GFX bridge */
			Device(AGPB) {
				Name(_ADR, 0x00010000)
				Method(_STA,0) {
					Return(0x0F)
				}
			}  /* end AGPB */

			/* The external GFX bridge */
			Device(PBR2) {
				Name(_ADR, 0x00020000)
				Method(_PRT,0) {
					If(GPIC){ Return(APS2) }   /* APIC mode */
					Return (PS2)               /* PIC Mode */
				} /* end _PRT */
			} /* end PBR2 */

			/* The external GFX bridge */
			Device(PBR3) {
				Name(_ADR, 0x00030000)
				Method(_PRT,0) {
					If(GPIC){ Return(APS3) }   /* APIC mode */
					Return (PS3)               /* PIC Mode */
				} /* end _PRT */
			} /* end PBR3 */

			Device(PBR4) {
				Name(_ADR, 0x00040000)
				Method(_PRT,0) {
					If(GPIC){ Return(APS4) }   /* APIC mode */
					Return (PS4)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR4 */

			Device(PBR5) {
				Name(_ADR, 0x00050000)
				Method(_PRT,0) {
					If(GPIC){ Return(APS5) }   /* APIC mode */
					Return (PS5)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR5 */

			Device(PBR6) {
				Name(_ADR, 0x00060000)
				Method(_PRT,0) {
					If(GPIC){ Return(APS6) }   /* APIC mode */
					Return (PS6)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR6 */

			/* The onboard EtherNet chip */
			Device(PBR7) {
				Name(_ADR, 0x00070000)
				Method(_PRT,0) {
					If(GPIC){ Return(APS7) }   /* APIC mode */
					Return (PS7)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR7 */

			Device(PE20) {
				Name(_ADR, 0x00150000)
				Method(_PRT,0) {
					If(GPIC){ Return(APE0) }   /* APIC mode */
					Return (PE0)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PE20 */
			Device(PE21) {
				Name(_ADR, 0x00150001)
				Method(_PRT,0) {
					If(GPIC){ Return(APE1) }   /* APIC mode */
					Return (PE1)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PE21 */
			Device(PE22) {
				Name(_ADR, 0x00150002)
				Method(_PRT,0) {
					If(GPIC){ Return(APE2) }   /* APIC mode */
					Return (APE2)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PE22 */
			Device(PE23) {
				Name(_ADR, 0x00150003)
				Method(_PRT,0) {
					If(GPIC){ Return(APE3) }   /* APIC mode */
					Return (PE3)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PE23 */

			/* Describe the Southbridge devices */
			Device(AZHD) {
				Name(_ADR, 0x00140002)
				OperationRegion(AZPD, PCI_Config, 0x00, 0x100)
					Field(AZPD, AnyAcc, NoLock, Preserve) {
					offset (0x42),
					NSDI, 1,
					NSDO, 1,
					NSEN, 1,
				}
			} /* end AZHD */

			Device(GEC) {
				Name(_ADR, 0x00140006)
			} /* end GEC */

			Device(UOH1) {
				Name(_ADR, 0x00120000)
				#include "acpi/usb.asl"
			} /* end UOH1 */

			Device(UOH3) {
				Name(_ADR, 0x00130000)
				#include "acpi/usb.asl"
			} /* end UOH3 */

			Device(UOH5) {
				Name(_ADR, 0x00160000)
				#include "acpi/usb.asl"
			} /* end UOH5 */

			Device(UEH1) {
				Name(_ADR, 0x00140005)
				#include "acpi/usb.asl"
			} /* end UEH1 */

			Device(UOH2) {
				Name(_ADR, 0x00120002)
				#include "acpi/usb.asl"
			} /* end UOH2 */

			Device(UOH4) {
				Name(_ADR, 0x00130002)
				#include "acpi/usb.asl"
			} /* end UOH4 */

			Device(UOH6) {
				Name(_ADR, 0x00160002)
				#include "acpi/usb.asl"
			} /* end UOH5 */

			Device(XHC0) {
				Name(_ADR, 0x00100000)
				#include "acpi/usb.asl"
			} /* end XHC0 */

			Device(XHC1) {
				Name(_ADR, 0x00100001)
				#include "acpi/usb.asl"
			} /* end XHC1 */

			Device(SBUS) {
				Name(_ADR, 0x00140000)
			} /* end SBUS */

			Device(LIBR) {
				Name(_ADR, 0x00140003)
				/* Real Time Clock Device */
				Device(RTC0) {
					Name(_HID, EISAID("PNP0B00"))	/* AT Real Time Clock (not PIIX4 compatible) */
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
#if 0
				Device(HPTM) {
					Name(_HID,EISAID("PNP0103"))
					Name(CRS,ResourceTemplate()	{
						Memory32Fixed(ReadOnly,0xFED00000, 0x00000400, HPT)	/* 1kb reserved space */
					})
					Method(_STA, 0) {
						Return(0x0F) /* sata is visible */
					}
					Method(_CRS, 0)	{
						CreateDwordField(CRS, ^HPT._BAS, HPBX)
						Store(HPBA, HPBX)
						Return(CRS)
					}
				} /* End Device(_SB.PCI0.LpcIsaBr.COPR) */
#endif
				Device (PS2M) {
					Name (_HID, EisaId ("PNP0F13"))
					Name (_CRS, ResourceTemplate ()	{
						IRQNoFlags () {12}
					})
					Method (_STA, 0, NotSerialized)	{
						And (FLG0, 0x04, Local0)
						If (LEqual (Local0, 0x04)) {
							Return (0x0F)
						} Else {
							Return (0x00)
						}
					}
				}

				Device (PS2K) {
					Name (_HID, EisaId ("PNP0303"))
					Name (_CRS, ResourceTemplate () {
						IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
						IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
						IRQNoFlags () {1}
					})
				}
			} /* end LIBR */

			Device(STCR) {
				Name(_ADR, 0x00110000)
				#include "acpi/sata.asl"
			} /* end STCR */

			/* Primary (and only) IDE channel */
			Device(IDEC) {
				Name(_ADR, 0x00140001)
				#include "acpi/ide.asl"
			} /* end IDEC */

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
					0x0D00,			/* range minimum */
					0xFFFF,			/* range maximum */
					0x0000,			/* translation */
					0xF300			/* length */
				)

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
		} /* End Device(PCI0)  */

		Device(PWRB) {	/* Start Power button device */
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_STA, 0x0B) /* sata is invisible */
		}
	} /* End \_SB scope */
}
/* End of ASL file */
