/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_PM_H_
#define _SOC_APOLLOLAKE_PM_H_

#include <stdint.h>

/* ACPI_BASE_ADDRESS */

#define PM1_STS			0x00
#define   WAK_STS		(1 << 15)
#define   PWRBTN_STS		(1 << 8)

#define PM1_EN			0x02
#define   PCIEXPWAK_DIS		(1 << 14)
#define   USB_WAKE_EN		(1 << 13)
#define   RTC_EN		(1 << 10)
#define   PWRBTN_EN     	(1 << 8)
#define   GBL_EN        	(1 << 5)
#define   TMROF_EN      	(1 << 0)

#define PM1_CNT			0x04
#define   SLP_EN		(1 << 13)
#define   SLP_TYP_SHIFT		10
#define   SLP_TYP		(7 << SLP_TYP_SHIFT)
#define    SLP_TYP_S0		0
#define    SLP_TYP_S3		5
#define    SLP_TYP_S4		6
#define    SLP_TYP_S5		7
#define   SCI_EN		(1 << 0)

#define PM1_TMR			0x08

#define SMI_EN			0x40

#define SMI_OCP_CSE		27
#define SMI_SPI			26
#define SMI_SPI_SSMI		25
#define SMI_SCC2		21
#define SMI_PCIE		20
#define SMI_SCS			19
#define SMI_HOST_SMBUS		18
#define SMI_XHCI		17
#define SMI_SMBUS		16
#define SMI_SERIRQ		15
#define SMI_PERIODIC		14
#define SMI_TCO			13
#define SMI_MCSMI		12
#define SMI_GPIO_UNLOCK_SSMI	11
#define SMI_GPIO		10
#define SMI_BIOS_RLS		7
#define SMI_SWSMI_TMR		6
#define SMI_APMC		5
#define SMI_SLP			4
#define SMI_LEGACY_USB		3
#define SMI_BIOS		2
#define SMI_EOS			1
#define SMI_GBL			0

#define   USB_EN		(1 << SMI_XHCI) /* Legacy USB2 SMI logic */
#define   PERIODIC_EN		(1 << SMI_PERIODIC) /* SMI on PERIODIC_STS in SMI_STS */
#define   TCO_EN		(1 << SMI_TCO) /* Enable TCO Logic (BIOSWE et al) */
#define   BIOS_RLS		(1 << SMI_BIOS_RLS) /* asserts SCI on bit set */
#define   SWSMI_TMR_EN		(1 << SMI_SWSMI_TMR) /* start software smi timer on bit set */
#define   APMC_EN		(1 << SMI_APMC) /* Writes to APM_CNT cause SMI# */
#define   SLP_SMI_EN		(1 << SMI_SLP) /* Write to SLP_EN in PM1_CNT asserts SMI# */
#define   BIOS_EN		(1 << SMI_BIOS) /* Assert SMI# on GBL_RLS bit */
#define   EOS			(1 << SMI_EOS) /* End of SMI (deassert SMI#) */
#define   GBL_SMI_EN		(1 << SMI_GBL) /* Global SMI Enable */

#define SMI_STS			0x44
#define GPE_CNTL		0x50
#define DEVACT_STS		0x4c
#define TCO_STS			0x64
#define   TCO_TIMEOUT		(1 << 3)
#define TCO1_CNT		0x68
#define   TCO_TMR_HLT		(1 << 11)

#define GPE0_REG_MAX		4
#define GPE0_REG_SIZE		32
#define GPE0_STS(x)		(0x20 + (x * 4))
#define GPE0_EN(x)		(0x30 + (x * 4))
#define   PME_B0_EN		(1 << 13)

/* Memory mapped IO registers behind PMC_BASE_ADDRESS */
#define PRSTS			0x1000
#define GEN_PMCON1		0x1020
#       define PWR_FLR		(1 << 16)
#       define SUS_PWR_FLR	(1 << 14)
#define GEN_PMCON2		0x1024
#       define RPS		(1 <<  2)
#define GEN_PMCON3		0x1028

/* Generic sleep state types */
#define SLEEP_STATE_S0		0
#define SLEEP_STATE_S3		3
#define SLEEP_STATE_S5		5

/* Track power state from reset to log events. */
struct chipset_power_state {
        uint16_t pm1_sts;
        uint16_t pm1_en;
        uint32_t pm1_cnt;
        uint32_t gpe0_sts[GPE0_REG_MAX];
        uint32_t gpe0_en[GPE0_REG_MAX];
        uint32_t tco_sts;
        uint32_t prsts;
        uint32_t gen_pmcon1;
        uint32_t gen_pmcon2;
        uint32_t gen_pmcon3;
        uint32_t prev_sleep_state;
} __attribute__((packed));

int fill_power_state(struct chipset_power_state *ps);
int chipset_prev_sleep_state(struct chipset_power_state *ps);

/* Power Management Utility Functions. */
uint32_t clear_smi_status(void);
uint16_t clear_pm1_status(void);
uint32_t clear_tco_status(void);
uint32_t clear_gpe_status(void);
void clear_pmc_status(void);
uint32_t get_smi_en(void);
void enable_smi(uint32_t mask);
void disable_smi(uint32_t mask);
void enable_pm1(uint16_t events);
void enable_pm1_control(uint32_t mask);
void disable_pm1_control(uint32_t mask);
void enable_gpe(uint32_t mask);
void disable_gpe(uint32_t mask);
void disable_all_gpe(void);

#endif
