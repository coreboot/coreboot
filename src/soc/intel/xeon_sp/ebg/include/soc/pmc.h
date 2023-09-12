/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_PMC_H_
#define _SOC_PMC_H_

/* PCI Configuration Space (D31:F2): PMC */
#define PWRMBASE		0x10
#define ABASE			0x20
/* Memory mapped IO registers in PMC */
#define GEN_PMCON_A		0x1020
#define  GBL_RST_STS		(1 << 24)
#define  DISB			(1 << 23)
#define  MS4V			(1 << 18)
#define  SUS_PWR_FLR		(1 << 16)
#define  PWR_FLR		(1 << 14)
#define  HOST_RST_STS		(1 << 9)
#define  PER_SMI_SEL_MASK	(3 << 1)
#define    SMI_RATE_64S		(0 << 1)
#define    SMI_RATE_32S		(1 << 1)
#define    SMI_RATE_16S		(2 << 1)
#define    SMI_RATE_8S		(3 << 1)
#define  SLEEP_AFTER_POWER_FAIL	(1 << 0)
#define GEN_PMCON_B		0x1024
#define  SLP_STR_POL_LOCK	(1 << 18)
#define  SMI_LOCK		(1 << 4)
#define  RTC_BATTERY_DEAD	(1 << 2)
#define PM_CFG			0x1818
#define  PMC_LOCK		(1 << 27)
#define PMSYNC_MISC_CFG		0x18c8
#define  PMSYNC_LOCK		(1 << 15)
#define PCH_PWRM_ACPI_TMR_CTL	0x18fc
#define  ACPI_TIM_DIS		(1 << 1)
#define GPIO_GPE_CFG		0x1920
#define  GPE0_DWX_MASK		0xf
#define GPE0_DW_SHIFT(x)	(4 * (x))
#define GBLRST_CAUSE0		0x1924
#define GBLRST_CAUSE1		0x1928
#define ACTL			0x1BD8
#define PMC_ACPI_CNT		0x1BD8
#define  PWRM_EN		(1 << 8)
#define  ACPI_EN		(1 << 7)
#define  SCI_IRQ_SEL		(7 << 0)
#define  SCI_IRQ_ADJUST		0
#define  SCIS_IRQ9		0
#define  SCIS_IRQ10		1
#define  SCIS_IRQ11		2
#define  SCIS_IRQ20		4
#define  SCIS_IRQ21		5
#define  SCIS_IRQ22		6
#define  SCIS_IRQ23		7
#define ST_PG_FDIS1		0x1e20
#define  ST_FDIS_LK		(1 << 31)
#define NST_PG_FDIS1		0x1e28
#define  NST_FDIS_DSP		(1 << 23)

#endif
