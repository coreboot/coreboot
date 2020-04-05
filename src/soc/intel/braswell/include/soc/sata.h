/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_SATA_H_
#define _SOC_SATA_H_

#define SATA_PORT_SUPPORT	0x03
#define SATA_PORT_MASK		0x3f

/* PCI Configuration Space */
#define SATA_PID		0x70
#define SATA_PID_NEXT		0xff00
#define SATA_PID_CID		0xff

#define SATA_MAP		0x90
#define SATA_MAP_SPD3		(1 << 11)
#define SATA_MAP_SPD2		(1 << 10)
#define SATA_MAP_SPD1		(1 << 9)
#define SATA_MAP_SPD0		(1 << 8)
#define SATA_MAP_SPD_MASK	(SATA_MAP_SPD0 | SATA_MAP_SPD1 \
				| SATA_MAP_SPD2 | SATA_MAP_SPD3)
#define SATA_MAP_SMS_RAID	0x40

#define SATA_PCS		0x92
#define SATA_PCS_ORM		(1 << 15)
#define SATA_PCS_PORT5		(1 << 5)
#define SATA_PCS_PORT4		(1 << 4)
#define SATA_PCS_PORT3		(1 << 3)
#define SATA_PCS_PORT2		(1 << 2)
#define SATA_PCS_PORT1		(1 << 1)
#define SATA_PCS_PORT0		(1 << 0)
#define SATA_PCS_PORTS	(SATA_PCS_PORT0 | SATA_PCS_PORT1 | SATA_PCS_PORT2 \
			| SATA_PCS_PORT3 | SATA_PCS_PORT4 | SATA_PCS_PORT5)

#define SATA_TM			0x94
#define SATA_TM_PCD5		(1 << 29)
#define SATA_TM_PCD4		(1 << 28)
#define SATA_TM_PCD3		(1 << 27)
#define SATA_TM_PCD2		(1 << 26)
#define SATA_TM_PCD1		(1 << 25)
#define SATA_TM_PCD0		(1 << 24)
#define SATA_TM_PCD_MASK	(SATA_TM_PCD0 | SATA_TM_PCD1 | SATA_TM_PCD2 \
				| SATA_TM_PCD3 | SATA_TM_PCD4 | SATA_TM_PCD5)

#define SATA_SIRI 0xa0
#define SATA_SIRD 0xa4

/* Memory Mapped I/O Space */
#define AHCI_GHC_CAP		0
#define AHCI_GHC_CAP_S64A	(1 << 31)
#define AHCI_GHC_CAP_SCQA	(1 << 30)
#define AHCI_GHC_CAP_SSNTF	(1 << 29)
#define AHCI_GHC_CAP_SMPS	(1 << 28)
#define AHCI_GHC_CAP_SSS	(1 << 27)
#define AHCI_GHC_CAP_SALP	(1 << 26)
#define AHCI_GHC_CAP_SAL	(1 << 25)
#define AHCI_GHC_CAP_SCLO	(1 << 24)
#define AHCI_GHC_CAP_ISS	0x00f00000
#define AHCI_GHC_CAP_ISS_GEN1	(1 << 20)
#define AHCI_GHC_CAP_ISS_GEN2	(2 << 20)
#define AHCI_GHC_CAP_ISS_GEN3	(3 << 20)
#define AHCI_GHC_CAP_SNZO	(1 << 19)
#define AHCI_GHC_CAP_SAM	(1 << 18)
#define AHCI_GHC_CAP_SMP	(1 << 17)
#define AHCI_GHC_CAP_FBSS	(1 << 16)
#define AHCI_GHC_CAP_PMD	(1 << 15)
#define AHCI_GHC_CAP_SSC	(1 << 14)
#define AHCI_GHC_CAP_PSC	(1 << 13)
#define AHCI_GHC_CAP_NCS	0x00000f00
#define AHCI_GHC_CAP_CCCS	(1 << 7)
#define AHCI_GHC_CAP_EMS	(1 << 6)
#define AHCI_GHC_CAP_SXS	(1 << 5)
#define AHCI_GHC_CAP_NP		0x0000001f

#define AHCI_HBA_CTRL		4
#define AHCI_HBA_CTRL_AE	(1 << 31)
#define AHCI_HBA_CTRL_MRSM	(1 << 2)
#define AHCI_HBA_CTRL_IE	(1 << 1)
#define AHCI_HBA_CTRL_HR	(1 << 0)

#define AHCI_GHC_PI		0x000c
#define AHCI_GHC_CAP2		0x0024
#define AHCI_GHC_CAP2_DESO	(1 << 5)
#define AHCI_GHC_CAP2_SADM	(1 << 4)
#define AHCI_GHC_CAP2_SDS	(1 << 3)
#define AHCI_GHC_CAP2_APST	(1 << 2)
#define AHCI_GHC_CAP2_BOH	(1 << 0)

#define AHCI_VSP		0x00a0
#define AHCI_VSP_SFMS		(1 << 6)
#define AHCI_VSP_PFS		(1 << 5)
#define AHCI_VSP_PT		(1 << 4)
#define AHCI_VSP_SRPIR		(1 << 3)

#define AHCI_SFM		0xc8
#define AHCI_SFM_OROM_UI	0x0c00
#define AHCI_SFM_OROM_UI_2SEC	0
#define AHCI_SFM_OROM_UI_4SEC	(1 << 10)
#define AHCI_SFM_OROM_UI_6SEC	(2 << 10)
#define AHCI_SFM_OROM_UI_8SEC	(3 << 10)
#define AHCI_SFM_SRT		(1 << 9)
#define AHCI_SFM_RRT_ESATA	(1 << 8)
#define AHCI_SFM_LED		(1 << 7)
#define AHCI_SFM_HDDUNLOCK	(1 << 6)
#define AHCI_SFM_OROM_UI_BANNER	(1 << 5)
#define AHCI_SFM_RRT		(1 << 4)
#define AHCI_SFM_R5		(1 << 3)
#define AHCI_SFM_R10		(1 << 2)
#define AHCI_SFM_R1		(1 << 1)
#define AHCI_SFM_R0		(1 << 0)

#define AHCI_PXCMD0		0x0118
#define AHCI_PXCMD1		0x0198

#define AHCI_PXCMD_ICC		0xf0000000
#define AHCI_PXCMD_ASP		(1 << 27)
#define AHCI_PXCMD_ALPE		(1 << 26)
#define AHCI_PXCMD_DLAE		(1 << 25)
#define AHCI_PXCMD_ATAPI	(1 << 24)
#define AHCI_PXCMD_APSTE	(1 << 23)
#define AHCI_PXCMD_FBSCP	(1 << 22)
#define AHCI_PXCMD_ESP		(1 << 21)
#define AHCI_PXCMD_CPD		(1 << 20)
#define AHCI_PXCMD_MPSP		(1 << 19)
#define AHCI_PXCMD_HPCP		(1 << 18)
#define AHCI_PXCMD_PMA		(1 << 17)
#define AHCI_PXCMD_CR		(1 << 15)
#define AHCI_PXCMD_FR		(1 << 14)
#define AHCI_PXCMD_MPSS		(1 << 13)
#define AHCI_PXCMD_CCS		0x00001f00
#define AHCI_PXCMD_PSP		(1 << 6)
#define AHCI_PXCMD_FRE		(1 << 4)
#define AHCI_PXCMD_CLO		(1 << 3)
#define AHCI_PXCMD_POD		(1 << 2)
#define AHCI_PXCMD_SUD		(1 << 1)
#define AHCI_PXCMD_ST		(1 << 0)

#endif	/* _SOC_SATA_H_ */
