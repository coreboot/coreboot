/* SPDX-License-Identifier: GPL-2.0-only */

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
Name(UDTT, Package(){ /* Udma timing table */
	120, 90, 60, 45, 30, 20, 15, 0	/* UDMA modes 0 -> 6 */
})

Name(MDTT, Package(){ /* MWDma timing table */
	480, 150, 120, 0	/* Legacy DMA modes 0 -> 2 */
})

Name(POTT, Package(){ /* Pio timing table */
	600, 390, 270, 180, 120, 0	/* PIO modes 0 -> 4 */
})

/* Some timing register value tables */
Name(MDRT, Package(){ /* MWDma timing register table */
	0x77, 0x21, 0x20, 0xFF	/* Legacy DMA modes 0 -> 2 */
})

Name(PORT, Package(){
	0x99, 0x47, 0x34, 0x22, 0x20, 0x99	/* PIO modes 0 -> 4 */
})

OperationRegion(ICRG, PCI_Config, 0x40, 0x20) /* ide control registers */
	Field(ICRG, AnyAcc, NoLock, Preserve)
{
	PPTS, 8,	/* Primary PIO Slave Timing */
	PPTM, 8,	/* Primary PIO Master Timing */
	OFFSET(0x04), PMTS, 8,	/* Primary MWDMA Slave Timing */
	PMTM, 8,	/* Primary MWDMA Master Timing */
	OFFSET(0x08), PPCR, 8,	/* Primary PIO Control */
	OFFSET(0x0A), PPMM, 4,	/* Primary PIO master Mode */
	PPSM, 4,	/* Primary PIO slave Mode */
	OFFSET(0x14), PDCR, 2,	/* Primary UDMA Control */
	OFFSET(0x16), PDMM, 4,	/* Primary UltraDMA Mode */
	PDSM, 4,	/* Primary UltraDMA Mode */
}

Method(GTTM, 1) /* get total time*/
{
	Local0 = Arg0 & 0x0F	/* Recovery Width */
	Local0++
	Local1 = Arg0 >> 4	/* Command Width */
	Local1++
	Return(30 * (Local0 + Local1))
}

Device(PRID)
{
	Name (_ADR, Zero)
	Method(_GTM, 0, Serialized)
	{
		NAME(OTBF, Buffer(20) { /* out buffer */
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
		})

		CreateDwordField(OTBF, 0, PSD0)   /* PIO spd0 */
		CreateDwordField(OTBF, 4, DSD0)   /* DMA spd0 */
		CreateDwordField(OTBF, 8, PSD1)   /* PIO spd1 */
		CreateDwordField(OTBF, 12, DSD1) /* DMA spd1 */
		CreateDwordField(OTBF, 16, BFFG) /* buffer flags */

		/* Just return if the channel is disabled */
		If (PPCR & 0x01) { /* primary PIO control */
			Return(OTBF)
		}

		/* Always tell them independent timing available and IOChannelReady used on both drives */
		BFFG |= 0x1A

		PSD0 = GTTM (PPTM) /* save total time of primary PIO master timing  to PIO spd0 */
		PSD1 = GTTM (PPTS) /* save total time of primary PIO slave Timing  to PIO spd1 */

		If (PDCR & 0x01) {	/* It's under UDMA mode */
			BFFG |= 0x01
			DSD0 = DerefOf(UDTT [PDMM])
		}
		Else {
			DSD0 = GTTM (PMTM) /* Primary MWDMA Master Timing,  DmaSpd0 */
		}

		If (PDCR & 0x02) {	/* It's under UDMA mode */
			BFFG |= 0x04
			DSD1 = DerefOf(UDTT [PDSM])
		}
		Else {
			DSD1 = GTTM (PMTS) /* Primary MWDMA Slave Timing,  DmaSpd0 */
		}

		Return(OTBF) /* out buffer */
	}				/* End Method(_GTM) */

	Method(_STM, 3, Serialized)
	{
		NAME(INBF, Buffer(20) { /* in buffer */
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
		})

		CreateDwordField(INBF, 0, PSD0)    /* PIO spd0 */
		CreateDwordField(INBF, 4, DSD0)   /* PIO spd0 */
		CreateDwordField(INBF, 8, PSD1)   /* PIO spd1 */
		CreateDwordField(INBF, 12, DSD1) /* DMA spd1 */
		CreateDwordField(INBF, 16, BFFG) /*buffer flag */

		Local0 = Match (POTT, MLE, PSD0, MTR, 0, 0)
		PPMM = Local0 % 5 /* Primary PIO master Mode */
		Local1 = Match (POTT, MLE, PSD1, MTR, 0, 0)
		PPSM = Local1 % 5 /* Primary PIO slave Mode */

		PPTM = DerefOf(PORT [Local0]) /* Primary PIO Master Timing */
		PPTS = DerefOf(PORT [Local1]) /* Primary PIO Slave Timing */

		If (BFFG & 0x01) {	/* Drive 0 is under UDMA mode */
			Local0 = Match (UDTT, MLE, DSD0, MTR, 0, 0)
			PDMM = Local0 % 7
			PDCR |= 0x01
		}
		Else {
			If (DSD0 != 0xFFFFFFFF) {
				Local0 = Match (MDTT, MLE, DSD0, MTR, 0, 0)
				PMTM = DerefOf(MDRT [Local0])
			}
		}

		If (BFFG & 0x04) {	/* Drive 1 is under UDMA mode */
			Local0 = Match (UDTT, MLE, DSD1, MTR, 0, 0)
			PDSM = Local0 % 7
			PDCR |= 0x02
		}
		Else {
			If (DSD1 != 0xFFFFFFFF) {
				Local0 = Match (MDTT, MLE, DSD1, MTR, 0, 0)
				PMTS = DerefOf(MDRT [Local0])
			}
		}
		/* Return(INBF) */
	}		/*End Method(_STM) */
	Device(MST)
	{
		Name(_ADR, 0)
		Method(_GTF, 0, Serialized) {
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

			CMDA = 0xA0
			CMDB = 0xA0
			CMDC = 0xA0

			POMD = PPMM | 0x08

			If (PDCR & 0x01) {
				DMMD = PDMM | 0x40
			}
			Else {
				Local0 = Match (MDTT, MLE, GTTM(PMTM), MTR, 0, 0)
				If (Local0 < 3) {
					DMMD = Local0 | 0x20
				}
			}
			Return(CMBF)
		}
	}		/* End Device(MST) */

	Device(SLAV)
	{
		Name(_ADR, 1)
		Method(_GTF, 0, Serialized) {
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

			CMDA = 0xB0
			CMDB = 0xB0
			CMDC = 0xB0

			POMD = PPSM | 0x08

			If (PDCR & 0x02) {
				DMMD = PDSM | 0x40
			}
			Else {
				Local0 = Match (MDTT, MLE, GTTM(PMTS), MTR, 0, 0)
				If (Local0 < 3) {
					DMMD = Local0 | 0x20
				}
			}
			Return(CMBF)
		}
	}			/* End Device(SLAV) */
}
