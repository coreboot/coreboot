/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012-2013 Advanced Micro Devices, Inc.
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

/* No IDE functionality */

#if 0
/*
Scope (_SB) {
	Device(PCI0) {
		Device(IDEC) {
			Name(_ADR, 0x00140001)
			#include "ide.asl"
		}
	}
}
*/

/* Some timing tables */
Name(UDTT, Package(){                   /* Udma timing table */
	120, 90, 60, 45, 30, 20, 15, 0      /* UDMA modes 0 -> 6 */
})

Name(MDTT, Package(){                   /* MWDma timing table */
	480, 150, 120, 0                    /* Legacy DMA modes 0 -> 2 */
})

Name(POTT, Package(){                   /* Pio timing table */
	600, 390, 270, 180, 120, 0          /* PIO modes 0 -> 4 */
})

/* Some timing register value tables */
Name(MDRT, Package(){                   /* MWDma timing register table */
	0x77, 0x21, 0x20, 0xFF              /* Legacy DMA modes 0 -> 2 */
})

Name(PORT, Package(){
	0x99, 0x47, 0x34, 0x22, 0x20, 0x99  /* PIO modes 0 -> 4 */
})

OperationRegion(ICRG, PCI_Config, 0x40, 0x20) /* ide control registers */
	Field(ICRG, AnyAcc, NoLock, Preserve)
{
	PPTS, 8,                            /* Primary PIO Slave Timing */
	PPTM, 8,                            /* Primary PIO Master Timing */
	OFFSET(0x04), PMTS, 8,              /* Primary MWDMA Slave Timing */
	PMTM, 8,                            /* Primary MWDMA Master Timing */
	OFFSET(0x08), PPCR, 8,              /* Primary PIO Control */
	OFFSET(0x0A), PPMM, 4,              /* Primary PIO master Mode */
	PPSM, 4,                            /* Primary PIO slave Mode */
	OFFSET(0x14), PDCR, 2,              /* Primary UDMA Control */
	OFFSET(0x16), PDMM, 4,              /* Primary UltraDMA Mode */
	PDSM, 4,                            /* Primary UltraDMA Mode */
}

Method(GTTM, 1)                         /* get total time*/
{
	Store(And(Arg0, 0x0F), Local0)      /* Recovery Width */
	Increment(Local0)
	Store(ShiftRight(Arg0, 4), Local1)  /* Command Width */
	Increment(Local1)
	Return(Multiply(30, Add(Local0, Local1)))
}

Device(PRID)
{
	Name (_ADR, Zero)
	Method(_GTM, 0)
	{
		NAME(OTBF, Buffer(20) {         /* out buffer */
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
		})

		CreateDwordField(OTBF, 0, PSD0) /* PIO spd0 */
		CreateDwordField(OTBF, 4, DSD0) /* DMA spd0 */
		CreateDwordField(OTBF, 8, PSD1) /* PIO spd1 */
		CreateDwordField(OTBF, 12, DSD1) /* DMA spd1 */
		CreateDwordField(OTBF, 16, BFFG) /* buffer flags */

		/* Just return if the channel is disabled */
		If(And(PPCR, 0x01)) {           /* primary PIO control */
			Return(OTBF)
		}

		/* Always tell them independent timing available and IOChannelReady used on both drives */
		Or(BFFG, 0x1A, BFFG)

		/* save total time of primary PIO master timing to PIO spd0 */
		Store(GTTM(PPTM), PSD0)
		/* save total time of primary PIO slave Timing to PIO spd1 */
		Store(GTTM(PPTS), PSD1)

		If(And(PDCR, 0x01)) {           /* It's under UDMA mode */
			Or(BFFG, 0x01, BFFG)
			Store(DerefOf(Index(UDTT, PDMM)), DSD0)
		}
		Else {
			Store(GTTM(PMTM), DSD0)     /* Primary MWDMA Master Timing, DmaSpd0 */
		}

		If(And(PDCR, 0x02)) {           /* It's under UDMA mode */
			Or(BFFG, 0x04, BFFG)
			Store(DerefOf(Index(UDTT, PDSM)), DSD1)
		}
		Else {
			Store(GTTM(PMTS), DSD1)     /* Primary MWDMA Slave Timing,  DmaSpd0 */
		}

		Return(OTBF)                    /* out buffer */
	}                                   /* End Method(_GTM) */

	Method(_STM, 3, NotSerialized)
	{
		NAME(INBF, Buffer(20) {         /* in buffer */
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
		})

		CreateDwordField(INBF, 0, PSD0) /* PIO spd0 */
		CreateDwordField(INBF, 4, DSD0) /* PIO spd0 */
		CreateDwordField(INBF, 8, PSD1) /* PIO spd1 */
		CreateDwordField(INBF, 12, DSD1) /* DMA spd1 */
		CreateDwordField(INBF, 16, BFFG) /*buffer flag */

		Store(Match(POTT, MLE, PSD0, MTR, 0, 0), Local0)
		Divide(Local0, 5, PPMM,)        /* Primary PIO master Mode */
		Store(Match(POTT, MLE, PSD1, MTR, 0, 0), Local1)
		Divide(Local1, 5, PPSM,)        /* Primary PIO slave Mode */

		Store(DerefOf(Index(PORT, Local0)), PPTM) /* Primary PIO Master Timing */
		Store(DerefOf(Index(PORT, Local1)), PPTS) /* Primary PIO Slave Timing */

		If(And(BFFG, 0x01)) {           /* Drive 0 is under UDMA mode */
			Store(Match(UDTT, MLE, DSD0, MTR, 0, 0), Local0)
			Divide(Local0, 7, PDMM,)
			Or(PDCR, 0x01, PDCR)
		}
		Else {
			If(LNotEqual(DSD0, 0xFFFFFFFF)) {
				Store(Match(MDTT, MLE, DSD0, MTR, 0, 0), Local0)
				Store(DerefOf(Index(MDRT, Local0)), PMTM)
			}
		}

		If(And(BFFG, 0x04)) {           /* Drive 1 is under UDMA mode */
			Store(Match(UDTT, MLE, DSD1, MTR, 0, 0), Local0)
			Divide(Local0, 7, PDSM,)
			Or(PDCR, 0x02, PDCR)
		}
		Else {
			If(LNotEqual(DSD1, 0xFFFFFFFF)) {
				Store(Match(MDTT, MLE, DSD1, MTR, 0, 0), Local0)
				Store(DerefOf(Index(MDRT, Local0)), PMTS)
			}
		}
		/* Return(INBF) */
	}		/*End Method(_STM) */
	Device(MST)
	{
		Name(_ADR, 0)
		Method(_GTF) {
			Name(CMBF, Buffer(21) {
				0x03, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xEF,
				0x03, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xEF,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF5
			})
			CreateByteField(CMBF, 1, POMD)
			CreateByteField(CMBF, 8, DMMD)
			CreateByteField(CMBF, 5, CMDA)
			CreateByteField(CMBF, 12, CMDB)
			CreateByteField(CMBF, 19, CMDC)

			Store(0xA0, CMDA)
			Store(0xA0, CMDB)
			Store(0xA0, CMDC)

			Or(PPMM, 0x08, POMD)

			If(And(PDCR, 0x01)) {
				Or(PDMM, 0x40, DMMD)
			}
			Else {
				Store(Match
				      (MDTT, MLE, GTTM(PMTM),
				       MTR, 0, 0), Local0)
				If(LLess(Local0, 3)) {
					Or(0x20, Local0, DMMD)
				}
			}
			Return(CMBF)
		}
	}                                   /* End Device(MST) */

	Device(SLAV)
	{
		Name(_ADR, 1)
		Method(_GTF) {
			Name(CMBF, Buffer(21) {
				0x03, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xEF,
				0x03, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xEF,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF5
			})
			CreateByteField(CMBF, 1, POMD)
			CreateByteField(CMBF, 8, DMMD)
			CreateByteField(CMBF, 5, CMDA)
			CreateByteField(CMBF, 12, CMDB)
			CreateByteField(CMBF, 19, CMDC)

			Store(0xB0, CMDA)
			Store(0xB0, CMDB)
			Store(0xB0, CMDC)

			Or(PPSM, 0x08, POMD)

			If(And(PDCR, 0x02)) {
				Or(PDSM, 0x40, DMMD)
			}
			Else {
				Store(Match
				      (MDTT, MLE, GTTM(PMTS),
				       MTR, 0, 0), Local0)
				If(LLess(Local0, 3)) {
					Or(0x20, Local0, DMMD)
				}
			}
			Return(CMBF)
		}
	}                                   /* End Device(SLAV) */
}
#endif
