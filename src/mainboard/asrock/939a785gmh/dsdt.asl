/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
	"DSDT.AML",	/* Output filename */
	"DSDT",		/* Signature */
	0x02,		/* DSDT Revision, needs to be 2 for 64bit */
	"ASROCK",	/* OEMID */
	"COREBOOT",	/* TABLE ID */
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	/* #include <arch/x86/acpi/debug.asl> */		/* Include global debug methods if needed */
	#include "northbridge/amd/amdk8/util.asl"

	Name(HPBA, 0xFED00000)	/* Base address of HPET table */

	Name(SSFG, 0x0D)		/* S1 support: bit 0, S2 Support: bit 1, etc. S0 & S5 assumed */

	Name(PMOD, One)	/* Assume APIC */

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
		, 0x00000008,		/* Index 6 */
		AAUD, 0x00000008,	/* Index 7 */
		AMOD, 0x00000008,	/* Index 8 */
		PINE, 0x00000008,	/* Index 9 */
		PINF, 0x00000008,	/* Index A */
		PING, 0x00000008,	/* Index B */
		PINH, 0x00000008,	/* Index C */
	}

	#include "acpi/routing.asl"

	Scope(\_SB) {

		Method(_PIC, 0x01, NotSerialized)
		{
			If (Arg0)
			{
				\_SB.CIRQ()
			}
			Store(Arg0, PMOD)
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
				if (PINA) {
					Return(0x0B) /* sata is invisible */
				} else {
					Return(0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTA._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKA\\_DIS\n") */
				Store(0, PINA)
			} /* End Method(_SB.INTA._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKA\\_PRS\n") */
				Return(IRQP)
			} /* Method(_SB.INTA._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKA\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				ShiftLeft(1, PINA, IRQN)
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
				Store(Local0, PINA)
			} /* End Method(_SB.INTA._SRS) */
		} /* End Device(INTA) */

		Device(INTB) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 2)

			Method(_STA, 0) {
				if (PINB) {
					Return(0x0B) /* sata is invisible */
				} else {
					Return(0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTB._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKB\\_DIS\n") */
				Store(0, PINB)
			} /* End Method(_SB.INTB._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKB\\_PRS\n") */
				Return(IRQP)
			} /* Method(_SB.INTB._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKB\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				ShiftLeft(1, PINB, IRQN)
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
				Store(Local0, PINB)
			} /* End Method(_SB.INTB._SRS) */
		} /* End Device(INTB)  */

		Device(INTC) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 3)

			Method(_STA, 0) {
				if (PINC) {
					Return(0x0B) /* sata is invisible */
				} else {
					Return(0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTC._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKC\\_DIS\n") */
				Store(0, PINC)
			} /* End Method(_SB.INTC._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKC\\_PRS\n") */
				Return(IRQP)
			} /* Method(_SB.INTC._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKC\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				ShiftLeft(1, PINC, IRQN)
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
				Store(Local0, PINC)
			} /* End Method(_SB.INTC._SRS) */
		} /* End Device(INTC)  */

		Device(INTD) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 4)

			Method(_STA, 0) {
				if (PIND) {
					Return(0x0B) /* sata is invisible */
				} else {
					Return(0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTD._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKD\\_DIS\n") */
				Store(0, PIND)
			} /* End Method(_SB.INTD._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKD\\_PRS\n") */
				Return(IRQP)
			} /* Method(_SB.INTD._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKD\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				ShiftLeft(1, PIND, IRQN)
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
				Store(Local0, PIND)
			} /* End Method(_SB.INTD._SRS) */
		} /* End Device(INTD)  */

		Device(INTE) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 5)

			Method(_STA, 0) {
				if (PINE) {
					Return(0x0B) /* sata is invisible */
				} else {
					Return(0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTE._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKE\\_DIS\n") */
				Store(0, PINE)
			} /* End Method(_SB.INTE._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKE\\_PRS\n") */
				Return(IRQP)
			} /* Method(_SB.INTE._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKE\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				ShiftLeft(1, PINE, IRQN)
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
				Store(Local0, PINE)
			} /* End Method(_SB.INTE._SRS) */
		} /* End Device(INTE)  */

		Device(INTF) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 6)

			Method(_STA, 0) {
				if (PINF) {
					Return(0x0B) /* sata is invisible */
				} else {
					Return(0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTF._STA) */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKF\\_DIS\n") */
				Store(0, PINF)
			} /* End Method(_SB.INTF._DIS) */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKF\\_PRS\n") */
				Return(PITF)
			} /* Method(_SB.INTF._PRS) */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKF\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				ShiftLeft(1, PINF, IRQN)
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
				Store(Local0, PINF)
			} /*  End Method(_SB.INTF._SRS) */
		} /* End Device(INTF)  */

		Device(INTG) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 7)

			Method(_STA, 0) {
				if (PING) {
					Return(0x0B) /* sata is invisible */
				} else {
					Return(0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTG._STA)  */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKG\\_DIS\n") */
				Store(0, PING)
			} /* End Method(_SB.INTG._DIS)  */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKG\\_PRS\n") */
				Return(IRQP)
			} /* Method(_SB.INTG._CRS)  */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKG\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				ShiftLeft(1, PING, IRQN)
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
				Store(Local0, PING)
			} /* End Method(_SB.INTG._SRS)  */
		} /* End Device(INTG)  */

		Device(INTH) {
			Name(_HID, EISAID("PNP0C0F"))
			Name(_UID, 8)

			Method(_STA, 0) {
				if (PINH) {
					Return(0x0B) /* sata is invisible */
				} else {
					Return(0x09) /* sata is disabled */
				}
			} /* End Method(_SB.INTH._STA)  */

			Method(_DIS ,0) {
				/* DBGO("\\_SB\\LNKH\\_DIS\n") */
				Store(0, PINH)
			} /* End Method(_SB.INTH._DIS)  */

			Method(_PRS ,0) {
				/* DBGO("\\_SB\\LNKH\\_PRS\n") */
				Return(IRQP)
			} /* Method(_SB.INTH._CRS)  */

			Method(_CRS ,0) {
				/* DBGO("\\_SB\\LNKH\\_CRS\n") */
				CreateWordField(IRQB, 0x1, IRQN)
				ShiftLeft(1, PINH, IRQN)
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
				Store(Local0, PINH)
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

	/* South Bridge */
	Scope(\_SB) { /* Start \_SB scope */
		#include <arch/x86/acpi/globutil.asl> /* global utility methods expected within the \_SB scope */

		/*  _SB.PCI0 */
		/* Note: Only need HID on Primary Bus */
		Device(PCI0) {

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

			/* Describe the Northbridge devices */
			Device(AMRT) {
				Name(_ADR, 0x00000000)
			} /* end AMRT */

			/* The internal GFX bridge */
			Device(AGPB) {
				Name(_ADR, 0x00010000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					Return (APR1)
				}
			}  /* end AGPB */

			/* The external GFX bridge */
			Device(PBR2) {
				Name(_ADR, 0x00020000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PMOD){ Return(APS2) }   /* APIC mode */
					Return (PS2)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR2 */

			/* GPP x1 */
			Device(PBR9) {
				Name(_ADR, 0x00090000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PMOD){ Return(APS9) }   /* APIC mode */
					Return (PS9)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR9 */

			/* ethernet */
			Device(PBRa) {
				Name(_ADR, 0x000A0000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PMOD){ Return(APSa) }   /* APIC mode */
					Return (PSa)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBRa */


			/* PCI slot 1, 2, 3 */
			Device(PIBR) {
				Name(_ADR, 0x00140004)
				Name(_PRW, Package() {0x18, 4})

				Method(_PRT, 0) {
					Return (PCIB)
				}
			}

			Device(LIBR) {
				Name(_ADR, 0x00140003)
				/* Method(_INI) {
				*	DBGO("\\_SB\\PCI0\\LpcIsaBr\\_INI\n")
				} */ /* End Method(_SB.SBRDG._INI) */

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

				Device(HPTM) {
					Name(_HID,EISAID("PNP0103"))
					Name(CRS,ResourceTemplate()	{
						Memory32Fixed(ReadOnly,0xFED00000, 0x00000400, HPT)	/* 1kb reserved space */
					})
					Method(_STA, 0) {
						Return(0x0F) /* sata is visible */
					}
					Method(_CRS, 0)	{
						CreateDwordField(CRS, ^HPT._BAS, HPBA)
						Store(HPBA, HPBA)
						Return(CRS)
					}
				} /* End Device(_SB.PCI0.LpcIsaBr.COPR) */
			} /* end LIBR */

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

		} /* End Device(PCI0)  */

	} /* End \_SB scope */
}
/* End of ASL file */
