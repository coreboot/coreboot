/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
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
#include <arch/ioapic.h>
#include <cpu/x86/lapic_def.h>
#include <southbridge/amd/sb600/sb600.h>

DefinitionBlock ("DSDT.aml", "DSDT", 2, "SIEMEN", "SITEMP  ", 0x20101005)
{
	/* Data to be patched by the BIOS during POST */
	/* Memory related values */
	Name(LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */

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

	Name(DSEN, 1)		// Display Output Switching Enable
	// Power notification

	/* PIC IRQ mapping registers, C00h-C01h */
	OperationRegion(PRQM, SystemIO, 0x00000C00, 0x00000002)
		Field(PRQM, ByteAcc, NoLock, Preserve) {
		PRQI, 0x00000008,
		PRQD, 0x00000008,  /* Offset: 1h */
	}
	IndexField(PRQI, PRQD, ByteAcc, NoLock, Preserve) {
		PINA, 0x00000008,	/* Index 0  */
		PINB, 0x00000008,	/* Index 1 */
		PINC, 0x00000008,	/* Index 2 */
		PIND, 0x00000008,	/* Index 3 */
		SINT, 0x00000008,	/*  Index 4 */
		Offset(0x09),
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

	/* Power Management I/O registers */
	OperationRegion(PIOR, SystemIO, 0x00000CD6, 0x00000002)
		Field(PIOR, ByteAcc, NoLock, Preserve) {
		PIOI, 0x00000008,
		PIOD, 0x00000008,
	}
	IndexField (PIOI, PIOD, ByteAcc, NoLock, Preserve) {
		Offset(0x00),	/* MiscControl */
		, 1,
		T1EE, 1,
		T2EE, 1,
		Offset(0x01),	/* MiscStatus */
		, 1,
		T1E, 1,
		T2E, 1,
		Offset(0x04),	/* SmiWakeUpEventEnable3 */
		, 7,
		SSEN, 1,
		Offset(0x07),	/* SmiWakeUpEventStatus3 */
		, 7,
		CSSM, 1,
		Offset(0x10),	/* AcpiEnable */
		, 6,
		PWDE, 1,
		Offset(0x1C),	/* ProgramIoEnable */
		, 3,
		MKME, 1,
		IO3E, 1,
		IO2E, 1,
		IO1E, 1,
		IO0E, 1,
		Offset(0x1D),	/* IOMonitorStatus */
		, 3,
		MKMS, 1,
		IO3S, 1,
		IO2S, 1,
		IO1S, 1,
		IO0S,1,
		Offset(0x20),	/* AcpiPmEvtBlk */
		APEB, 16,
		Offset(0x36),	/* GEvtLevelConfig */
		, 6,
		ELC6, 1,
		ELC7, 1,
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
		Offset(0x3B),	/* PMEStatus1 */
		GP0S, 1,
		GM4S, 1,
		GM5S, 1,
		APS, 1,
		GM6S, 1,
		GM7S, 1,
		GP2S, 1,
		STSS, 1,
		Offset(0x55),	/* SoftPciRst */
		SPRE, 1,
		, 1,
		, 1,
		PNAT, 1,
		PWMK, 1,
		PWNS, 1,

		/* 	Offset(0x61), */	/*  Options_1 */
		/* 		,7,  */
		/* 		R617,1, */

		Offset(0x65),	/* UsbPMControl */
		, 4,
		URRE, 1,
		Offset(0x68),	/* MiscEnable68 */
		, 3,
		TMTE, 1,
		, 1,
		Offset(0x92),	/* GEVENTIN */
		, 7,
		E7IS, 1,
		Offset(0x96),	/* GPM98IN */
		G8IS, 1,
		G9IS, 1,
		Offset(0x9A),	/* EnhanceControl */
		,7,
		HPDE, 1,
		Offset(0xA8),	/* PIO7654Enable */
		IO4E, 1,
		IO5E, 1,
		IO6E, 1,
		IO7E, 1,
		Offset(0xA9),	/* PIO7654Status */
		IO4S, 1,
		IO5S, 1,
		IO6S, 1,
		IO7S, 1,
	}

	/* PM1 Event Block
	* First word is PM1_Status, Second word is PM1_Enable
	*/
	OperationRegion(P1EB, SystemIO, APEB, 0x04)
		Field(P1EB, ByteAcc, NoLock, Preserve) {
		TMST, 1,
		,    3,
		BMST,    1,
		GBST,   1,
		Offset(0x01),
		PBST, 1,
		, 1,
		RTST, 1,
		, 3,
		PWST, 1,
		SPWS, 1,
		Offset(0x02),
		TMEN, 1,
		, 4,
		GBEN, 1,
		Offset(0x03),
		PBEN, 1,
		, 1,
		RTEN, 1,
		, 3,
		PWDA, 1,
	}

	External(\NVSA)

	OperationRegion (GVAR, SystemMemory, \NVSA, 0x100)
	Field (GVAR, ByteAcc, NoLock, Preserve)
	{
		Offset (0x00),
		OSYS,	16,
		LINX,	16,
		PCBA,   32,
		MPEN,	8
	}

	Name (IOLM,0xe0000000)

#include "acpi/platform.asl"

	Scope(\_SB) {

		/* PCIe Configuration Space for 16 busses */
		OperationRegion(PCFG, SystemMemory, PCBA, 0x2000000) /* PCIe reserved space for 31 busses */
			Field(PCFG, ByteAcc, NoLock, Preserve) {
			Offset(0x00090024),	/* Byte offset to SATA BAR5 register 24h - Bus 0, Device 18, Function 0 */
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

		OperationRegion(BAR5, SystemMemory, STB5, 0x1000)
			Field(BAR5, AnyAcc, NoLock, Preserve)
			{
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
#include "acpi/event.asl"
#include "acpi/routing.asl"
#include "acpi/usb.asl"

	/* System Bus */
	Scope(\_SB)
	{
		/* Start \_SB scope */

#include "acpi/globutil.asl"

		Device(PWRB) {	/* Start Power button device */
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_PRW, Package () {3, 0x04})	/* wake from S1-S4 */
			Name(_STA, 0x0B) /* sata is invisible */
		}
		/*  _SB.PCI0 */
		/* Note: Only need HID on Primary Bus */
		Device(PCI0)
		{
			External (MMIO)
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

			Device (MEMR)
			{
				Name (_HID, EisaId ("PNP0C02"))
				Name (MEM1, ResourceTemplate ()
				{
				Memory32Fixed (ReadWrite,
			0x00000000,         // Address Base
			0x00000000,         // Address Length
			_Y1A)
				Memory32Fixed (ReadWrite,
			0x00000000,         // Address Base
			0x00000000,         // Address Length
			_Y1B)
				})
				Method (_CRS, 0, NotSerialized)
				{
				CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y1A._BAS, MB01)
				CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y1A._LEN, ML01)
				CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y1B._BAS, MB02)
				CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y1B._LEN, ML02)
				If (PCIF)
				{
			Store (IO_APIC_ADDR, MB01)
			Store (LOCAL_APIC_ADDR, MB02)
			Store (0x1000, ML01)
			Store (0x1000, ML02)
				}

				Return (MEM1)
				}
			}

			Method(_PRT,0) {
				If(PCIF){ Return(APR0) }   /* APIC mode */
				Return (PR0)                  /* PIC Mode */
			} /* end _PRT */

			OperationRegion (BAR1, PCI_Config, 0x14, 0x04)
			Field (BAR1, ByteAcc, NoLock, Preserve)
			{
				Z009,   32
			}

			/* Describe the Northbridge devices */
			Device(AMRT) {
				Name(_ADR, 0x00000000)
			} /* end AMRT */

			/* The internal GFX bridge */
			Device(AGPB) {
				Name(_ADR, 0x00010000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) { Return (APR1) }

				Device (VGA)
				{
				Name (_ADR, 0x00050000)
					Method (_DOS, 1)
					{
						/* Windows 2000 and Windows XP call _DOS to enable/disable
						 * Display Output Switching during init and while a switch
						 * is already active
						*/
						Store (And(Arg0, 7), DSEN)
					}
				Method (_STA, 0, NotSerialized)
				{
			Return (0x0F)
				}
				}
			}  /* end AGPB */

			/* The external GFX bridge */
			Device(PBR2) {
				Name(_ADR, 0x00020000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PCIF){ Return(APS2) }   /* APIC mode */
					Return (PS2)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR2 */

			/* Dev3 is also an external GFX bridge */

			Device(PBR4) {
				Name(_ADR, 0x00040000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PCIF){ Return(APS4) }   /* APIC mode */
					Return (PS4)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR4 */

			Device(PBR5) {
				Name(_ADR, 0x00050000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PCIF){ Return(APS5) }   /* APIC mode */
					Return (PS5)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR5 */

			Device(PBR6) {
				Name(_ADR, 0x00060000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PCIF){ Return(APS6) }   /* APIC mode */
					Return (PS6)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR6 */

			/* The onboard EtherNet chip */
			Device(PBR7) {
				Name(_ADR, 0x00070000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PCIF){ Return(APS7) }   /* APIC mode */
					Return (PS7)               /* PIC Mode */
				} /* end _PRT */
			} /* end PBR7 */

			/* PCI slot 1 */
			Device(PIBR) {
				Name(_ADR, 0x00140004)
				Name(_PRW, Package() {4, 5}) //  Phoenix doeas it so
				Method(_PRT, 0) {
					If(PCIF){ Return(AP2P) }  /* APIC Mode */
					Return (PCIB)             /* PIC Mode */
				}
			}

			/* Describe the Southbridge devices */
			Device(SATA) {
				Name(_ADR, 0x00120000)
#include "acpi/sata.asl"
			} /* end SATA */

			Device(UOH1) {
				Name(_ADR, 0x00130000)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH1 */

			Device(UOH2) {
				Name(_ADR, 0x00130001)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH2 */

			Device(UOH3) {
				Name(_ADR, 0x00130002)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH3 */

			Device(UOH4) {
				Name(_ADR, 0x00130003)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH4 */

			Device(UOH5) {
				Name(_ADR, 0x00130004)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH5 */

			Device(UEH1) {
				Name(_ADR, 0x00130005)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UEH1 */

			Device(SBUS) {
				Name(_ADR, 0x00140000)
			} /* end SBUS */

			/* Primary (and only) IDE channel */
			Device(IDEC) {
				Name(_ADR, 0x00140001)
				#include "acpi/ide.asl"
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
					If(LEqual(LINX,1)){   /* If we are running Linux */
						Store(zero, NSEN)
						Store(one, NSDO)
						Store(one, NSDI)
					}
				}
			} /* end AZHD */

			Device(LPC0)
			{
				Name (_ADR, 0x00140003)
				Mutex (PSMX, 0x00)

				/* PIC IRQ mapping registers, C00h-C01h */
				OperationRegion(PRQM, SystemIO, 0x00000C00, 0x00000002)
				Field(PRQM, ByteAcc, NoLock, Preserve) {
					PRQI, 0x00000008,
					PRQD, 0x00000008,  /* Offset: 1h */
				}

				IndexField(PRQI, PRQD, ByteAcc, NoLock, Preserve) {
					PINA, 0x00000008,	/* Index 0  */
					PINB, 0x00000008,	/* Index 1 */
					PINC, 0x00000008,	/* Index 2 */
					PIND, 0x00000008,	/* Index 3 */
					SINT, 0x00000008,	/*  Index 4 */
					Offset(0x09),
					PINE, 0x00000008,	/* Index 9 */
					PINF, 0x00000008,	/* Index A */
					PING, 0x00000008,	/* Index B */
					PINH, 0x00000008,	/* Index C */
				}

				Method(CIRQ, 0x00, NotSerialized)
				{
					Store(0, PINA)
					Store(0, PINB)
					Store(0, PINC)
					Store(0, PIND)
					Store(0, SINT)
					Store(0, PINE)
					Store(0, PINF)
					Store(0, PING)
					Store(0, PINH)
				}

				Name(IRQB, ResourceTemplate(){
					IRQ(Level,ActiveLow,Shared){10,11}
				})

				Name(IRQP, ResourceTemplate(){
					IRQ(Level,ActiveLow,Exclusive){3, 4, 5, 7}
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
						Store(0, PINA)
					} /* End Method(_SB.INTA._DIS) */

					Method(_PRS ,0) {
						Return(IRQB) // Return(IRQP)
					} /* Method(_SB.INTA._PRS) */

					Method(_CRS ,0) {
						Store (IRQB, Local0) //
						CreateWordField(Local0, 0x1, IRQ0)
						ShiftLeft(1, PINA, IRQ0)
						Return(Local0)
					} /* Method(_SB.INTA._CRS) */
					Method(_SRS, 1) {
						CreateWordField(ARG0, 1, IRQ0)
						/* Use lowest available IRQ */
						FindSetRightBit(IRQ0, Local0)
						Decrement (Local0)
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
						Store(0, PINB)
					} /* End Method(_SB.INTB._DIS) */

					Method(_PRS ,0) {
						Return(IRQB) // Return(IRQP)
					} /* Method(_SB.INTB._PRS) */

					Method(_CRS ,0) {
						Store (IRQB, Local0) // {10,11}
						CreateWordField(Local0, 0x1, IRQ0)
						ShiftLeft(1, PINB, IRQ0)
						Return(Local0)
					} /* Method(_SB.INTB._CRS) */

					Method(_SRS, 1) {
						CreateWordField(ARG0, 1, IRQ0)
						/* Use lowest available IRQ */
						FindSetRightBit(IRQ0, Local0)
						Decrement(Local0)
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
						Store(0, PINC)
					} /* End Method(_SB.INTC._DIS) */

					Method(_PRS ,0) {
						Return(IRQB) // Return(IRQP)
					} /* Method(_SB.INTC._PRS) */

					Method(_CRS ,0) {
						Store (IRQB, Local0) // {10,11}
						CreateWordField(Local0, 0x1, IRQ0)
						ShiftLeft(1, PINC, IRQ0)
						Return(Local0)
					} /* Method(_SB.INTC._CRS) */

					Method(_SRS, 1) {
						CreateWordField(ARG0, 1, IRQ0)
						/* Use lowest available IRQ */
						FindSetRightBit(IRQ0, Local0)
						Decrement(Local0)
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
						Store(0, PIND)
					} /* End Method(_SB.INTD._DIS) */

					Method(_PRS ,0) {
						Return(IRQB) // Return(IRQP)
					} /* Method(_SB.INTD._PRS) */

					Method(_CRS ,0) {
						Store (IRQB, Local0) // {10,11}
						CreateWordField(Local0, 0x1, IRQ0)
						ShiftLeft(1, PIND, IRQ0)
						Return(Local0)
					} /* Method(_SB.INTD._CRS) */

					Method(_SRS, 1) {
						CreateWordField(ARG0, 1, IRQ0)
						/* Use lowest available IRQ */
						FindSetRightBit(IRQ0, Local0)
						Decrement(Local0)
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
						Store(0, PINE)
					} /* End Method(_SB.INTE._DIS) */

					Method(_PRS ,0) {
						Return(IRQB) // Return(IRQP)
					}

					Method(_CRS ,0) {
						Store (IRQB, Local0) // {10,11}
						CreateWordField(Local0, 0x1, IRQ0)
						ShiftLeft(1, PINE, IRQ0)
						Return(Local0)
					} /* Method(_SB.INTE._CRS) */

					Method(_SRS, 1) {
						CreateWordField(ARG0, 1, IRQ0)
						/* Use lowest available IRQ */
						FindSetRightBit(IRQ0, Local0)
						Decrement(Local0)
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
						Store(0, PINF)
					} /* End Method(_SB.INTF._DIS) */

					Method(_PRS ,0) {
						Return(IRQB) // Return(PITF)
					} /* Method(_SB.INTF._PRS) */

					Method(_CRS ,0) {
						Store (IRQB, Local0) // {10,11}
						CreateWordField(Local0, 0x1, IRQ0)
						ShiftLeft(1, PINF, IRQ0)
						Return(Local0)
					} /* Method(_SB.INTF._CRS) */

					Method(_SRS, 1) {
						CreateWordField(ARG0, 1, IRQ0)
						/* Use lowest available IRQ */
						FindSetRightBit(IRQ0, Local0)
						Decrement(Local0)
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
						Store(0, PING)
					} /* End Method(_SB.INTG._DIS)  */

					Method(_PRS ,0) {
						Return(IRQB) // Return(IRQP)
					} /* Method(_SB.INTG._CRS)  */

					Method(_CRS ,0) {
						Store (IRQB, Local0) // {10,11}
						CreateWordField(Local0, 0x1, IRQ0)
						ShiftLeft(1, PING, IRQ0)
						Return(Local0)
					} /* Method(_SB.INTG._CRS)  */

					Method(_SRS, 1) {
						CreateWordField(ARG0, 1, IRQ0)
						/* Use lowest available IRQ */
						FindSetRightBit(IRQ0, Local0)
						Decrement(Local0)
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
						Store(0, PINH)
					} /* End Method(_SB.INTH._DIS)  */

					Method(_PRS ,0) {
						Return(IRQB) // Return(IRQP)
					} /* Method(_SB.INTH._CRS)  */

					Method(_CRS ,0) {
						Store (IRQB, Local0) // {10,11}
						CreateWordField(Local0, 0x1, IRQ0)
						ShiftLeft(1, PINH, IRQ0)
						Return(Local0)
					} /* Method(_SB.INTH._CRS)  */

					Method(_SRS, 1) {
						CreateWordField(ARG0, 1, IRQ0)
						/* Use lowest available IRQ */
						FindSetRightBit(IRQ0, Local0)
						Decrement(Local0)
						Store(Local0, PINH)
					} /* End Method(_SB.INTH._SRS)  */
				} /* End Device(INTH)   */


				/* Real Time Clock Device */
				Device(RTC0) {
					Name(_HID, EISAID("PNP0B00"))	/* AT Real Time Clock (not PIIX4 compatible)*/
					Name(_CRS, ResourceTemplate() {
						IRQ (Edge, ActiveHigh, Exclusive, ) {8}
						IO(Decode16,0x0070, 0x0070, 1, 2)
						/* IO(Decode16,0x0070, 0x0070, 0, 4) */
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.RTC0) */

				Device(TMR) {	/* Timer */
					Name(_HID,EISAID("PNP0100"))	/* System Timer */
					Name(_CRS, ResourceTemplate() {
			IRQ (Edge, ActiveHigh, Exclusive, ) {0}
						IO(Decode16, 0x0040, 0x0040, 1, 4)
						/* IO(Decode16, 0x0048, 0x0048, 0, 4) */
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.TMR) */

				Device(SPKR) {	/* Speaker */
					Name(_HID,EISAID("PNP0800"))	/* AT style speaker */
					Name(_CRS, ResourceTemplate() {
						IO(Decode16, 0x0061, 0x0061, 1, 1)
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.SPKR) */

				Device(PIC) {
					Name(_HID,EISAID("PNP0000"))	/* AT Interrupt Controller */
					Name(_CRS, ResourceTemplate() {
						IRQ (Edge, ActiveHigh, Exclusive, ) {2}
						IO(Decode16,0x0020, 0x0020, 1, 2)
						IO(Decode16,0x00A0, 0x00A0, 0, 2)
						/* IO(Decode16, 0x00D0, 0x00D0, 0x10, 0x02) */
						/* IO(Decode16, 0x04D0, 0x04D0, 0x10, 0x02) */
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.PIC) */

				Device(MAD) { /* 8257 DMA */
					Name(_HID,EISAID("PNP0200"))	/* Hardware Device ID */
					Name(_CRS, ResourceTemplate() {
						DMA(Compatibility,NotBusMaster,Transfer8_16){4}
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
						IO(Decode16, 0x00F0, 0x00F0, 1, 0x10)
						IRQ (Edge, ActiveHigh, Exclusive, ) {13}
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.COPR) */

				Device(HPET) { /* HPET */
					Name(_HID,EISAID("PNP0103"))
					Name(CRS,ResourceTemplate()	{
						Memory32Fixed(ReadOnly, HPET_BASE_ADDRESS, 0x00000400, HPT)	/* 1kb reserved space */
					})
					Method(_STA, 0) {
						Return(0x0F) /* HPET is visible */
					}
					Method(_CRS, 0)	{
						Return(CRS)
					}
				}

				Device (KBC0)
				{
				Name (_HID, EisaId ("PNP0303"))
				Name (_CRS, ResourceTemplate ()
				{
			IO (Decode16,
				0x0060,             // Range Minimum
				0x0060,             // Range Maximum
				0x01,               // Alignment
				0x01,               // Length
				)
			IO (Decode16,
				0x0064,             // Range Minimum
				0x0064,             // Range Maximum
				0x01,               // Alignment
				0x01,               // Length
				)
			IRQ (Edge, ActiveHigh, Exclusive, ) {1}
				})
				}

				Device (MSE0)
				{
				Name (_HID, EisaId ("PNP0F13"))
				Name (_CRS, ResourceTemplate ()
				{
			IRQ (Edge, ActiveHigh, Exclusive, ) {12}
				})
				}
			} /* end LPC0 */

			Device(ACAD) {
				Name(_ADR, 0x00140005)
				Name (_PRW, Package (0x02)
				{
				0x0C,
				0x04
				})
			} /* end Ac97audio */

			Device(ACMD) {
				Name(_ADR, 0x00140006)
				Name (_PRW, Package (0x02)
				{
				0x0C,
				0x04
				})
			} /* end Ac97modem */

			/* ITE IT8712F Support */
			OperationRegion (IOID, SystemIO, 0x2E, 0x02)	/* sometimes it is 0x4E */
				Field (IOID, ByteAcc, NoLock, Preserve)
				{
					SIOI,   8,    SIOD,   8		/* 0x2E and 0x2F */
				}

			IndexField (SIOI, SIOD, ByteAcc, NoLock, Preserve)
			{
					Offset (0x07),
				LDN,	8,	/* Logical Device Number */
					Offset (0x20),
				CID1,	8,	/* Chip ID Byte 1, 0x87 */
				CID2,	8,	/* Chip ID Byte 2, 0x12 */
					Offset (0x30),
				ACTR,	8,	/* Function activate */
					Offset (0xF0),
				APC0,	8,	/* APC/PME Event Enable Register */
				APC1,	8,	/* APC/PME Status Register */
				APC2,	8,      /* APC/PME Control Register 1 */
				APC3,	8,	/* Environment Controller Special Configuration Register */
				APC4,	8	/* APC/PME Control Register 2 */
			}

			/* Enter the IT8712F MB PnP Mode */
			Method (EPNP)
			{
				Store(0x87, SIOI)
				Store(0x01, SIOI)
				Store(0x55, SIOI)
				Store(0x55, SIOI) /* IT8712F magic number */
			}
			/* Exit the IT8712F MB PnP Mode */
			Method (XPNP)
			{
				Store (0x02, SIOI)
				Store (0x02, SIOD)
			}

			/*
			 * Keyboard PME is routed to SB600 Gevent3. We can wake
			 * up the system by pressing the key.
			 */
	        Method (SIOS, 1)
			{
				/* We only enable KBD PME for S5. */
				If (LLess (Arg0, 0x05))
				{
					EPNP()
					/* DBGO("IT8712F\n") */

					Store (0x4, LDN)
					Store (One, ACTR)  /* Enable EC */
					/*
					Store (0x4, LDN)
					Store (0x04, APC4)
					*/  /* falling edge. which mode? Not sure. */

					Store (0x4, LDN)
					Store (0x08, APC1) /* clear PME status, Use 0x18 for mouse & KBD */
					Store (0x4, LDN)
					Store (0x08, APC0) /* enable PME, Use 0x18 for mouse & KBD */

					XPNP()
				}
			}
			Method (SIOW, 0)
			{
				EPNP()
				Store (0x4, LDN)
				Store (Zero, APC0) /* disable keyboard PME */
				Store (0x4, LDN)
				Store (0xFF, APC1) /* clear keyboard PME status */
				XPNP()
			}

/* ############################################################################################### */
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

				Memory32Fixed(READONLY, 0x000A0000, 0x00020000, VGAM) 	/* VGA memory space */
				Memory32Fixed(READONLY, 0x000C0000, 0x00020000, EMM1)	/* Assume C0000-E0000 empty */
				Memory32Fixed(READONLY, 0x000E0000, 0x00020000, RDBS)   /* BIOS ROM area */

				/* DRAM Memory from 1MB to TopMem */
				DWORDMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite, 0, 0, 0, 0x00, 1, ,, EMM2)
				WORDIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange, 0x00, 0x0D00, 0xffff, 0x00, 0xf300)
			}) /* End Name(_SB.PCI0.CRES) */

			Method(_CRS, 0) {

				/* DBGO("\\_SB\\PCI0\\_CRS\n") */

				CreateDWordField(CRES, ^EMM1._BAS, EM1B)
				CreateDWordField(CRES, ^EMM1._LEN, EM1L)

				CreateDWordField(CRES, ^EMM2._MIN, EM2B)
				CreateDWordField(CRES, ^EMM2._MAX, EM2E)
				CreateDWordField(CRES, ^EMM2._LEN, EM2L)

				Store(TOM1, EM2B)
				Subtract(IOLM, 1, EM2E)
				Subtract(IOLM, TOM1, EM2L)

				If(LGreater(LOMH, 0xC0000)){
					Store(0xC0000, EM1B)	/* Hole above C0000 and below E0000 */
					Subtract(LOMH, 0xC0000, EM1L)	/* subtract start, assumes allocation from C0000 going up */
				}

				Return(CRES) /* note to change the Name buffer */
			}
/* ########################################################################################## */
		} /* End Device(PCI0)  */
	} /* End \_SB scope */

	Scope(\_SI) {
		Method(_SST, 1) {
			/* DBGO("\\_SI\\_SST\n") */
			/* DBGO("   New Indicator state: ") */
			/* DBGO(Arg0) */
			/* DBGO("\n") */
		}
	} /* End Scope SI */

#include <southbridge/amd/cimx/sb800/acpi/smbus.asl>
#include "acpi/thermal.asl"
}
