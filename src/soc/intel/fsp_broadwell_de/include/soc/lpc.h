/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#ifndef _SOC_LPC_H_
#define _SOC_LPC_H_

/* LPC Interface Bridge PCI Configuration Registers */
#define REVID			0x08
#define PIRQ_RCR1		0x60
#define PIRQ_RCR2		0x68
#define GEN_PMCON_1		0xA0
#define GEN_PMCON_2		0xA2
#define GEN_PMCON_3		0xA4
#define   RTC_PWR_STS		(1 << 2)

/* Default IO range claimed by the LPC device. The upper bound is exclusive. */
#define LPC_DEFAULT_IO_RANGE_LOWER 0
#define LPC_DEFAULT_IO_RANGE_UPPER 0x1000

/* IO Mapped registers behind ACPI_BASE_ADDRESS */
#define PM1_STS			0x00
#define   WAK_STS		(1 << 15)
#define   PCIEXPWAK_STS		(1 << 14)
#define   USB_STS		(1 << 13)
#define   PRBTNOR_STS		(1 << 11)
#define   RTC_STS		(1 << 10)
#define   PWRBTN_STS		(1 << 8)
#define   GBL_STS		(1 << 5)
#define   TMROF_STS		(1 << 0)
#define PM1_EN			0x02
#define   PCIEXPWAK_DIS		(1 << 14)
#define   RTC_EN		(1 << 10)
#define   PWRBTN_EN		(1 << 8)
#define   GBL_EN		(1 << 5)
#define   TMROF_EN		(1 << 0)
#define PM1_CNT			0x04
#define   SLP_EN		(1 << 13)
#define   SLP_TYP_SHIFT		10
#define   SLP_TYP		(7 << SLP_TYP_SHIFT)
#define    SLP_TYP_S0		0
#define    SLP_TYP_S1		1
#define    SLP_TYP_S3		5
#define    SLP_TYP_S4		6
#define    SLP_TYP_S5		7
#define   GBL_RLS		(1 << 2)
#define   BM_RLD		(1 << 1)
#define   SCI_EN		(1 << 0)
#define PM1_TMR			0x08
#define GPE0_STS		0x20
#define   PCI_EXP_STS		(1 << 9)
#define   RI_STS		(1 << 8)
#define   SMB_WAK_STS		(1 << 7)
#define   TCOSCI_STS		(1 << 6)
#define   SWGPE_STS		(1 << 2)
#define   HOT_PLUG_STS		(1 << 1)
#define GPE0_EN			0x28
#define SMI_EN			0x30
#define SMI_STS			0x34
#define ALT_GPIO_SMI		0x38
#define UPRWC			0x3c
#define   UPRWC_WR_EN		(1 << 1) // USB Per-Port Registers Write Enable
#define GPE_CTRL		0x40
#define PM2A_CNT_BLK		0x50
#define TCO_RLD			0x60
#define TCO_STS			0x64
#define   SECOND_TO_STS		(1 << 17)
#define   TCO_TIMEOUT		(1 << 3)
#define TCO1_CNT		0x68
#define   TCO_LOCK		(1 << 12)
#define   TCO_TMR_HALT		(1 << 11)
#define TCO_TMR			0x70

#endif /* _SOC_LPC_H_ */