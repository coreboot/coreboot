/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CANNONLAKE_PMC_H_
#define _SOC_CANNONLAKE_PMC_H_

#include <device/device.h>

extern struct device_operations pmc_ops;

/* PCI Configuration Space (D31:F2): PMC */
#define  PWRMBASE		0x10
#define  ABASE			0x20

/* Memory mapped IO registers in PMC */
#define GEN_PMCON_A			0x1020
#define   DC_PP_DIS			(1 << 30)
#define   DSX_PP_DIS			(1 << 29)
#define   AG3_PP_EN			(1 << 28)
#define   SX_PP_EN			(1 << 27)
#define   ALLOW_ICLK_PLL_SD_INC0	(1 << 26)
#define   GBL_RST_STS			(1 << 24)
#define   DISB				(1 << 23)
#define   ALLOW_OPI_PLL_SD_INC0		(1 << 22)
#define   MEM_SR			(1 << 21)
#define   ALLOW_SPXB_CG_INC0		(1 << 20)
#define   ALLOW_L1LOW_C0		(1 << 19)
#define   MS4V				(1 << 18)
#define   ALLOW_L1LOW_OPI_ON		(1 << 17)
#define   SUS_PWR_FLR			(1 << 16)
#define   PME_B0_S5_DIS			(1 << 15)
#define   PWR_FLR			(1 << 14)
#define   ALLOW_L1LOW_BCLKREQ_ON	(1 << 13)
#define   DIS_SLP_X_STRCH_SUS_UP	(1 << 12)
#define   SLP_S3_MIN_ASST_WDTH_MASK	(3 << 10)
#define     SLP_S3_MIN_ASST_WDTH_60USEC	(0 << 10)
#define     SLP_S3_MIN_ASST_WDTH_1MS	(1 << 10)
#define     SLP_S3_MIN_ASST_WDTH_50MS	(2 << 10)
#define     SLP_S3_MIN_ASST_WDTH_2S	(3 << 10)
#define   HOST_RST_STS			(1 << 9)
#define   ESPI_SMI_LOCK			(1 << 8)
#define   S4MAW_MASK			(3 << 4)
#define     S4MAW_1S			(1 << 4)
#define     S4MAW_2S			(2 << 4)
#define     S4MAW_3S			(3 << 4)
#define     S4MAW_4S			(0 << 4)
#define     S4ASE			(1 << 3)
#define   PER_SMI_SEL_MASK		(3 << 1)
#define     SMI_RATE_64S		(0 << 1)
#define     SMI_RATE_32S		(1 << 1)
#define     SMI_RATE_16S		(2 << 1)
#define     SMI_RATE_8S			(3 << 1)
#define   SLEEP_AFTER_POWER_FAIL	(1 << 0)

#define GEN_PMCON_B			0x1024
#define   SLP_STR_POL_LOCK		(1 << 18)
#define   ACPI_BASE_LOCK		(1 << 17)
#define   PM_DATA_BAR_DIS		(1 << 16)
#define   WOL_EN_OVRD			(1 << 13)
#define   BIOS_PCI_EXP_EN		(1 << 10)
#define   PWRBTN_LVL			(1 << 9)
#define   SMI_LOCK			(1 << 4)
#define   RTC_BATTERY_DEAD		(1 << 2)

#define ETR				0x1048
#define   CF9_LOCK			(1 << 31)
#define   CF9_GLB_RST			(1 << 20)

#define SSML				0x104C
#define    SSML_SSL_DS			(0 << 0)
#define    SSML_SSL_EN			(1 << 0)

#define SSMC				0x1050
#define    SSMC_SSMS			(1 << 0)

#define SSMD				0x1054
#define    SSMD_SSD_MASK		(0xffff << 0)

#define PRSTS				0x1810

#define S3_PWRGATE_POL			0x1828
#define    S3DC_GATE_SUS		(1 << 1)
#define    S3AC_GATE_SUS		(1 << 0)

#define S4_PWRGATE_POL			0x182c
#define    S4DC_GATE_SUS		(1 << 1)
#define    S4AC_GATE_SUS		(1 << 0)

#define S5_PWRGATE_POL			0x1830
#define    S5DC_GATE_SUS		(1 << 15)
#define    S5AC_GATE_SUS		(1 << 14)

#define DSX_CFG				0x1834
#define   REQ_CNV_NOWAKE_DSX		(1 << 4)
#define    REQ_BATLOW_DSX		(1 << 3)
#define   DSX_EN_WAKE_PIN		(1 << 2)
#define   DSX_DIS_AC_PRESENT_PD	(1 << 1)
#define   DSX_EN_LAN_WAKE_PIN		(1 << 0)
#define DSX_CFG_MASK			(0x1f << 0)

#define PMSYNC_TPR_CFG			0x18C4
#define   PCH2CPU_TPR_CFG_LOCK		(1 << 31)
#define   PCH2CPU_TT_EN			(1 << 26)

#define PCH_PWRM_ACPI_TMR_CTL		0x18FC
#define  ACPI_TIM_DIS			(1 << 1)
#define GPIO_GPE_CFG			0x1920
#define  GPE0_DWX_MASK			0xf
#define  GPE0_DW_SHIFT(x)		(4*(x))

#define SLP_S0_RES			0x193c

#if CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
/*
 * The values for GPIO_CFG in Intel Document #572235 are incorrect.
 * These values now match what is used by the Intel CoffeeLake FSP,
 * please do not modify them.
 */
#define PMC_GPP_A			0x0
#define PMC_GPP_B			0x1
#define PMC_GPP_C			0x2
#define PMC_GPP_D			0x3
#define PMC_GPP_E			0xA
#define PMC_GPP_F			0xB
#define PMC_GPP_G			0x4
#define PMC_GPP_H			0x9
#define PMC_GPP_I			0xC
#define PMC_GPP_J			0xD
#define PMC_GPP_K			0x8
#define PMC_GPD				0x7
#else
#define  PMC_GPP_A			0x0
#define  PMC_GPP_B			0x1
#define  PMC_GPP_C			0xD
#define  PMC_GPP_D			0x4
#define  PMC_GPP_E			0xE
#define  PMC_GPP_F			0x5
#define  PMC_GPP_G			0x2
#define  PMC_GPP_H			0x6
#define  PMC_GPD			0xA
#endif

#define GBLRST_CAUSE0			0x1924
#define   GBLRST_CAUSE0_THERMTRIP	(1 << 5)
#define GBLRST_CAUSE1			0x1928
#define HPR_CAUSE0			0x192C

#define LTR_IGN				0x1B0C
#define   IGN_GBE			(1 << 3)

#define CPPMVRIC			0x1B1C
#define   XTALSDQDIS			(1 << 22)

#define CPPMVRIC2			0x1B4C
#define   ADSPOSCDIS			(1 << 22)

#define IRQ_REG				ACTL
#define SCI_IRQ_ADJUST			0
#define ACTL				0x1BD8
#define   PWRM_EN			(1 << 8)
#define   ACPI_EN			(1 << 7)
#define   SCI_IRQ_SEL			(7 << 0)

#define  SCIS_IRQ9			0
#define  SCIS_IRQ10			1
#define  SCIS_IRQ11			2
#define  SCIS_IRQ20			4
#define  SCIS_IRQ21			5
#define  SCIS_IRQ22			6
#define  SCIS_IRQ23			7

void pmc_set_afterg3(int s5pwr);

#endif
