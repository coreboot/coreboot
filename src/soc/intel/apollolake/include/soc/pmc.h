/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APOLLOLAKE_PMC_H_
#define _SOC_APOLLOLAKE_PMC_H_

/* Memory mapped IO registers behind PMC_BASE_ADDRESS */
#define PRSTS			0x1000
#define GEN_PMCON1		0x1020
#define GEN_PMCON_A		GEN_PMCON1
#define  COLD_BOOT_STS		(1 << 27)
#define  COLD_RESET_STS		(1 << 26)
#define  WARM_RESET_STS		(1 << 25)
#define  GBL_RST_STS		(1 << 24)
#define  SRS			(1 << 20)
#define  MS4V			(1 << 18)
#define  SUS_PWR_FLR		(1 << 16)
#define  PWR_FLR		(1 << 14)
#define  RPS			(1 << 2)
#define  SLEEP_AFTER_POWER_FAIL	(1 << 0)
#define GEN_PMCON1_CLR1_BITS	(COLD_BOOT_STS | COLD_RESET_STS | \
				 WARM_RESET_STS | GBL_RST_STS | \
				 SRS | MS4V)
#define GEN_PMCON2		0x1024
#define GEN_PMCON_B		GEN_PMCON2
#define  LPC_LPB_CLK_CTRL	((1 << 11) | (1 << 12) | (1 << 13))
#define  BIOS_PCI_EXP_EN	(1 << 10)
#define  PWRBTN_LVL		(1 <<  9)
#define  SMI_LOCK		(1 <<  4)
#define  PER_SMI_SEL_MASK	(3 <<  0)
#define    SMI_RATE_64S		(0 <<  0)
#define    SMI_RATE_32S		(1 <<  0)
#define    SMI_RATE_16S		(2 <<  0)
#define    SMI_RATE_8S		(3 <<  0)

#define GEN_PMCON3		0x1028
#define  SLP_S3_ASSERT_WIDTH_SHIFT	10
#define  SLP_S3_ASSERT_MASK	(0x3 << SLP_S3_ASSERT_WIDTH_SHIFT)
#define  SLP_S3_ASSERT_60_USEC	0x0
#define  SLP_S3_ASSERT_1_MSEC	0x1
#define  SLP_S3_ASSERT_50_MSEC	0x2
#define  SLP_S3_ASSERT_2_SEC	0x3
#define ETR			0x1048
#define CF9_LOCK		(1 << 31)
#define  CF9_GLB_RST		(1 << 20)
#define GPIO_GPE_CFG		0x1050
#define  GPE0_DWX_MASK		0xf
#define GPE0_DW_SHIFT(x)	(4 + 4*(x))

#if CONFIG(SOC_INTEL_GEMINILAKE)
#define PMC_GPE_AUDIO_31_0	9
#define PMC_GPE_N_95_64		8
#define PMC_GPE_N_63_32		7
#define PMC_GPE_N_31_0		6
#define PMC_GPE_NW_127_96	5
#define PMC_GPE_NW_95_64	4
#define PMC_GPE_NW_63_32	3
#define PMC_GPE_NW_31_0		2
#define PMC_GPE_SCC_63_32	1
#define PMC_GPE_SCC_31_0	0
#else  /*For APL*/
#define  PMC_GPE_SW_31_0	0
#define  PMC_GPE_SW_63_32	1
#define  PMC_GPE_NW_31_0	3
#define  PMC_GPE_NW_63_32	4
#define  PMC_GPE_NW_95_64	5
#define  PMC_GPE_N_31_0		6
#define  PMC_GPE_N_63_32	7
#define  PMC_GPE_W_31_0		9
#endif

#define IRQ_REG			0x106C
#define SCI_IRQ_ADJUST		24
#define SCI_IRQ_SEL		(255 << SCI_IRQ_ADJUST)
#define SCIS_IRQ9		9
#define SCIS_IRQ10		10
#define SCIS_IRQ11		11
#define SCIS_IRQ20		20
#define SCIS_IRQ21		21
#define SCIS_IRQ22		22
#define SCIS_IRQ23		23

#endif
