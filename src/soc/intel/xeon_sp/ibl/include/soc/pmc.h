/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_PMC_H_
#define _SOC_PMC_H_

/* PCI Configuration Space (D31:F2): PMC */
#define PWRMBASE		0x10
#define ABASE			0x20
/* Memory mapped IO registers in PMC */
#define GEN_PMCON_A		0x1020
#define  GBL_RST_STS		(1 << 24)
#define  MS4V			(1 << 18)
#define  SUS_PWR_FLR		(1 << 16)
#define  PWR_FLR		(1 << 14)
#define  PER_SMI_SEL_MASK	(3 << 1)
#define    SMI_RATE_64S		(0 << 1)
#define  SLEEP_AFTER_POWER_FAIL	(1 << 0)
#define GEN_PMCON_B		0x1024
#define  SLP_STR_POL_LOCK	(1 << 18)
#define  RTC_BATTERY_DEAD	(1 << 2)
#define PM_CFG			0x1818
#define  PMC_LOCK		(1 << 27)
#define PCH_PWRM_ACPI_TMR_CTL	0x18fc
#define  ACPI_TIM_DIS		(1 << 1)
#define GPIO_GPE_CFG		0x1920
#define  GPE0_DWX_MASK		0xf
#define GPE0_DW_SHIFT(x)	(4 * (x))
#define GBLRST_CAUSE0		0x1924
#define GBLRST_CAUSE1		0x1928
#define PMC_ACPI_CNT		0x44
#define  SCI_IRQ_SEL		(7 << 0)
#define  SCI_IRQ_ADJUST		0
#define  SCIS_IRQ9		0
#define  SCIS_IRQ10		1
#define  SCIS_IRQ11		2
#define  SCIS_IRQ20		4
#define  SCIS_IRQ21		5
#define  SCIS_IRQ22		6
#define  SCIS_IRQ23		7

#endif
