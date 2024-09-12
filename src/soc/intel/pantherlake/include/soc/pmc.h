/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_PMC_H_
#define _SOC_PANTHERLAKE_PMC_H_
#include <device/device.h>

extern struct device_operations pmc_ops;

/* PCI Configuration Space (D31:F2): PMC */
#define  PWRMBASE		0x10
#define  ABASE			0x20

/* General PM Configuration A */
#define GEN_PMCON_A			0x1020
#define   DC_PP_DIS			BIT(30)
#define   DSX_PP_DIS			BIT(29)
#define   AG3_PP_EN			BIT(28)
#define   SX_PP_EN			BIT(27)
#define   ALLOW_ICLK_PLL_SD_INC0	BIT(26)
#define   GBL_RST_STS			BIT(24)
#define   DISB				BIT(23)
#define   ALLOW_OPI_PLL_SD_INC0		BIT(22)
#define   MEM_SR			BIT(21)
#define   ALLOW_SPXB_CG_INC0		BIT(20)
#define   ALLOW_L1LOW_C0		BIT(19)
#define   MS4V				BIT_FLAG_32(18)
#define   ALLOW_L1LOW_OPI_ON		BIT(17)
#define   SUS_PWR_FLR			BIT(16)
#define   PME_B0_S5_DIS			BIT(15)
#define   PWR_FLR			BIT(14)
#define   ALLOW_L1LOW_BCLKREQ_ON	BIT(13)
#define   DIS_SLP_X_STRCH_SUS_UP	BIT(12)
#define   SLP_S3_MIN_ASST_WDTH_MASK	(3 << 10)
#define     SLP_S3_MIN_ASST_WDTH_60USEC	(0 << 10)
#define     SLP_S3_MIN_ASST_WDTH_1MS	BIT(10)
#define     SLP_S3_MIN_ASST_WDTH_50MS	(2 << 10)
#define     SLP_S3_MIN_ASST_WDTH_2S	(3 << 10)
#define   HOST_RST_STS			BIT(9)
#define   ESPI_SMI_LOCK			BIT(8)
#define   S4MAW_MASK			(3 << 4)
#define     S4MAW_1S			BIT(4)
#define     S4MAW_2S			(2 << 4)
#define     S4MAW_3S			(3 << 4)
#define     S4MAW_4S			(0 << 4)
#define     S4ASE			BIT(3)
#define   PER_SMI_SEL_MASK		(3 << 1)
#define     SMI_RATE_64S		(0 << 1)
#define     SMI_RATE_32S		BIT(1)
#define     SMI_RATE_16S		(2 << 1)
#define     SMI_RATE_8S			(3 << 1)
#define   SLEEP_AFTER_POWER_FAIL	BIT(0)

/* General PM Configuration B */
#define GEN_PMCON_B			0x1024
#define   ST_FDIS_LOCK			BIT(21)
#define   SLP_STR_POL_LOCK		BIT(18)
#define   ACPI_BASE_LOCK		BIT(17)
#define   PM_DATA_BAR_DIS		BIT(16)
#define   WOL_EN_OVRD			BIT(13)
#define   BIOS_PCI_EXP_EN		BIT(10)
#define   PWRBTN_LVL			BIT(9)
#define   SMI_LOCK			BIT(4)
#define   RTC_BATTERY_DEAD		BIT(2)

/* Extended Test Mode Register */
#define ETR				0x1048
#define   CF9_LOCK			BIT(31)
#define   CF9_GLB_RST			BIT(20)

/* Set strap message lock */
#define SSML				0x104C
#define   SSML_SSL_DS			(0 << 0)
#define   SSML_SSL_EN			BIT(0)

/* Set strap msg control */
#define SSMC				0x1050
#define   SSMC_SSMS			BIT(0)

/* Set strap message data */
#define SSMD				0x1054
#define   SSMD_SSD_MASK			(0xffff << 0)

/* Power and Reset Status */
#define PRSTS				0x1810

/* Power Management Configuration */
#define PM_CFG				0x1818
#define   PM_CFG_DBG_MODE_LOCK		BIT(27)
#define   PM_CFG_XRAM_READ_DISABLE	BIT(22)

/* S3 Power Gating Policies */
#define S3_PWRGATE_POL			0x1828
#define   S3DC_GATE_SUS			BIT(1)
#define   S3AC_GATE_SUS			BIT(0)

/* S4 power gating policies */
#define S4_PWRGATE_POL			0x182c
#define   S4DC_GATE_SUS			BIT(1)
#define   S4AC_GATE_SUS			BIT(0)

/* S5 power gating policies */
#define S5_PWRGATE_POL			0x1830
#define   S5DC_GATE_SUS			BIT(15)
#define   S5AC_GATE_SUS			BIT(14)

/* Deep Sx configuration */
#define DSX_CFG				0x1834
#define   REQ_CNV_NOWAKE_DSX		BIT(4)
#define   REQ_BATLOW_DSX		BIT(3)
#define   DSX_EN_WAKE_PIN		BIT(2)
#define   DSX_DIS_AC_PRESENT_PD	BIT(1)
#define   DSX_EN_LAN_WAKE_PIN		BIT(0)
#define DSX_CFG_MASK			(0x1f << 0)

#define PMSYNC_TPR_CFG			0x18C4
#define   PCH2CPU_TPR_CFG_LOCK		BIT(31)
#define   PCH2CPU_TT_EN			BIT(26)

/* ACPI Timer Control */
#define PCH_PWRM_ACPI_TMR_CTL		0x18FC
#define   ACPI_TIM_DIS			BIT(1)
#define GPIO_GPE_CFG			0x1920
#define   GPE0_DWX_MASK			0xf
#define   GPE0_DW_SHIFT(x)		(4*(x))

/* GPIO community groups */
#define PMC_GPP_V		0x0
#define PMC_GPP_C		0x1
#define PMC_GPP_F		0x2
#define PMC_GPP_E		0x3
#define PMC_GPP_A		0x4
#define PMC_GPP_H		0x5
#define PMC_GPP_VGPIO		0x6
#define PMC_GPP_B		0x7
#define PMC_GPP_D		0x8
#define PMC_GPP_S		0x9

/* Global reset causes 0 */
#define GBLRST_CAUSE0			0x1924
#define   GBLRST_CAUSE0_THERMTRIP	BIT(5)

/* Global reset causes 1 */
#define GBLRST_CAUSE1			0x1928

/* Host partition reset causes */
#define HPR_CAUSE0			0x192C
#define   HPR_CAUSE0_MI_HRPD		BIT(10)
#define   HPR_CAUSE0_MI_HRPC		BIT(9)
#define   HPR_CAUSE0_MI_HR		BIT(8)

/* Sleep S0 residency */
#define SLP_S0_RES			0x193c

#define CPPMVRIC			0x1B1C
#define   XTALSDQDIS			BIT(22)

#define IRQ_REG				ACTL
#define SCI_IRQ_ADJUST			0

/* ACPI Control */
#define ACTL				0x1BD8
#define   PWRM_EN			BIT(8)
#define   ACPI_EN			BIT(7)
#define   SCI_IRQ_SEL			(7 << 0)

#define  SCIS_IRQ9			0
#define  SCIS_IRQ10			1
#define  SCIS_IRQ11			2
#define  SCIS_IRQ20			4
#define  SCIS_IRQ21			5
#define  SCIS_IRQ22			6
#define  SCIS_IRQ23			7

#endif /* _SOC_PANTHERLAKE_PMC_H_ */
