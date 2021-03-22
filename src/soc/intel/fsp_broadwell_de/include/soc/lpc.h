/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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

#include <arch/acpi.h>

/* LPC Interface Bridge PCI Configuration Registers */
#define GPIO_BASE_ADR_OFFSET	0x48
#define GPIO_CTRL_OFFSET	0x4c
#define   GPIO_DECODE_ENABLE	(1 << 4)
#define REVID			0x08
#define PIRQ_RCR1		0x60
#define SIRQ_CNTL		0x64
#define   SIRQ_EN		0x80
#define   SIRQ_MODE_QUIET	0x00
#define   SIRQ_MODE_CONT	0x40
#define PIRQ_RCR2		0x68
#define LPC_IO_DEC		0x80
#define LPC_EN			0x82
#define LPC_GEN1_DEC		0x84
#define LPC_GEN2_DEC		0x88
#define LPC_GEN3_DEC		0x8c
#define LPC_GEN4_DEC		0x90
#define LGMR			0x98 /* LPC Generic Memory Range */
#define GEN_PMCON_1		0xA0
#define   SMI_LOCK		(1 << 4)
#define   SMI_LOCK_GP6		(1 << 5)
#define   SMI_LOCK_GP22		(1 << 6)
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
#define  XHCI_SMI_EN		(1 << 31)
#define  ME_SMI_EN		(1 << 30)
#define  GPIO_UNLOCK_SMI_EN	(1 << 27)
#define  INTEL_USB2_EN		(1 << 18)
#define  LEGACY_USB2_EN		(1 << 17)
#define  PERIODIC_EN		(1 << 14)
#define  TCO_EN			(1 << 13)
#define  MCSMI_EN		(1 << 11)
#define  BIOS_RLS		(1 <<  7)
#define  SWSMI_TMR_EN		(1 <<  6)
#define  APMC_EN		(1 <<  5)
#define  SLP_SMI_EN		(1 <<  4)
#define  LEGACY_USB_EN		(1 <<  3)
#define  BIOS_EN		(1 <<  2)
#define  EOS			(1 <<  1)
#define  GBL_SMI_EN		(1 <<  0)
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

/* PM1_CNT */
void enable_pm1_control(uint32_t mask);
void disable_pm1_control(uint32_t mask);

/* PM1 */
uint16_t clear_pm1_status(void);
void enable_pm1(uint16_t events);
uint32_t clear_smi_status(void);

/* SMI */
void enable_smi(uint32_t mask);
void disable_smi(uint32_t mask);

#endif /* _SOC_LPC_H_ */
