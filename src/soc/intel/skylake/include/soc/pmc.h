/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PMC_H_
#define _SOC_PMC_H_

/* PCI Configuration Space (D31:F2): PMC */
#define ABASE			0x40
#define ACTL			0x44
#define  PWRM_EN		(1 << 8)
#define  ACPI_EN		(1 << 7)
#define  SCI_IRQ_SEL		(7 << 0)
#define  SCIS_IRQ9		0
#define  SCIS_IRQ10		1
#define  SCIS_IRQ11		2
#define  SCIS_IRQ20		4
#define  SCIS_IRQ21		5
#define  SCIS_IRQ22		6
#define  SCIS_IRQ23		7
#define PWRMBASE		0x48
#define GEN_PMCON_A		0xa0
#define  DC_PP_DIS		(1 << 30)
#define  DSX_PP_DIS		(1 << 29)
#define  AG3_PP_EN		(1 << 28)
#define  SX_PP_EN		(1 << 27)
#define  DISB			(1 << 23)
#define  MEM_SR			(1 << 21)
#define  MS4V			(1 << 18)
#define  GBL_RST_STS		(1 << 16)
#define  ALLOW_ICLK_PLL_SD_INC0	(1 << 15)
#define  MPHY_CRICLK_GATE_OVER	(1 << 14)
#define  ALLOW_OPI_PLL_SD_INC0	(1 << 13)
#define  ALLOW_SPXB_CG_INC0	(1 << 12)
#define  BIOS_PCI_EXP_EN	(1 << 10)
#define  PWRBTN_LVL		(1 << 9)
#define  ALLOW_L1LOW_C0		(1 << 7)
#define  ALLOW_L1LOW_OPI_ON	(1 << 6)
#define  SMI_LOCK		(1 << 4)
#define GEN_PMCON_B		0xa4
#define  SLP_STR_POL_LOCK	(1 << 18)
#define  ACPI_BASE_LOCK		(1 << 17)
#define  SUS_PWR_FLR		(1 << 14)
#define  WOL_EN_OVRD		(1 << 13)
#define  DIS_SLP_X_STRCH_SUS_UP		(1 << 12)
#define  SLP_S3_MIN_ASST_WDTH_MASK	(0x3 << 10)
#define  SLP_S3_MIN_ASST_WDTH_60USEC	(0 << 10)
#define  SLP_S3_MIN_ASST_WDTH_1MS	(1 << 10)
#define  SLP_S3_MIN_ASST_WDTH_50MS	(2 << 10)
#define  SLP_S3_MIN_ASST_WDTH_2S	(3 << 10)
#define  HOST_RST_STS		(1 << 9)
#define  S4MAW_MASK		(0x3 << 4)
#define  S4MAW_1S		(1 << 4)
#define  S4MAW_2S		(2 << 4)
#define  S4MAW_3S		(3 << 4)
#define  S4MAW_4S		(0 << 4)
#define  S4ASE			(1 << 3)
#define  RTC_BATTERY_DEAD	(1 << 2)
#define  PWR_FLR		(1 << 1)
#define  SLEEP_AFTER_POWER_FAIL	(1 << 0)
#define ETR3			0xac
#define  ETR3_CF9LOCK		(1 << 31)
#define  ETR3_CF9GR		(1 << 20)

/* Memory mapped IO registers in PMC */
#define S3_PWRGATE_POL		0x28
#define  S3DC_GATE_SUS		(1 << 1)
#define  S3AC_GATE_SUS		(1 << 0)
#define S4_PWRGATE_POL		0x2c
#define  S4DC_GATE_SUS		(1 << 1)
#define  S4AC_GATE_SUS		(1 << 0)
#define S5_PWRGATE_POL		0x30
#define  S5DC_GATE_SUS		(1 << 15)
#define  S5AC_GATE_SUS		(1 << 14)
#define DSX_CFG			0x34
#define  DSX_CFG_MASK		0x7
#define  DSX_EN_WAKE_PIN	(1 << 2)
#define  DSX_DIS_AC_PRESENT_PD	(1 << 1)
#define  DSX_EN_LAN_WAKE_PIN	(1 << 0)
#define PMSYNC_TPR_CFG		0xc4
#define  PMSYNC_LOCK		(1 << 31)
#define PCH_PWRM_ACPI_TMR_CTL	0xfc
#define  ACPI_TIM_DIS		(1 << 1)
#define GPIO_GPE_CFG		0x120
#define  GPE0_DWX_MASK		0xf
#define GPE0_DW_SHIFT(x)	(4*(x))
#define GBLRST_CAUSE0		0x124
#define GBLRST_CAUSE1		0x128
#define CPPMVRIC		0x31c
#define  XTALSDQDIS		(1 << 22)
#endif
