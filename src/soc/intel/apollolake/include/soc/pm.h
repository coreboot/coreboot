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
#include <compiler.h>
#include <arch/acpi.h>
#include <soc/gpe.h>
#include <soc/iomap.h>

/* ACPI_BASE_ADDRESS */

#define PM1_STS			0x00
#define   WAK_STS		(1 << 15)
#define   PCIEXPWAK_STS		(1 << 14)
#define   PRBTNOR_STS		(1 << 11)
#define   RTC_STS		(1 << 10)
#define   PWRBTN_STS		(1 << 8)
#define   GBL_STS		(1 << 5)

#define PM1_EN			0x02
#define   PCIEXPWAK_DIS		(1 << 14)
#define   RTC_EN		(1 << 10)
#define   PWRBTN_EN		(1 << 8)
#define   GBL_EN		(1 << 5)

#define PM1_CNT			0x04
#define   SCI_EN		(1 << 0)

#define PM1_TMR			0x08

#define SMI_EN			0x40

#define SMI_ESPI		28 /* This bit is present in GLK*/
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

#if IS_ENABLED(CONFIG_SOC_ESPI)
#define   ESPI_SMI_EN	(1 << SMI_ESPI) /* Valid for GLK with ESPI */
#else
#define   ESPI_SMI_EN	0
#endif
#define   USB_EN	(1 << SMI_XHCI) /* Legacy USB2 SMI logic */
#define   PERIODIC_EN	(1 << SMI_PERIODIC) /* SMI on PERIODIC_STS in SMI_STS */
#define   TCO_EN	(1 << SMI_TCO) /* Enable TCO Logic (BIOSWE et al) */
#define   GPIO_EN	(1 << SMI_GPIO) /* Enable GPIO SMI */
#define   BIOS_RLS	(1 << SMI_BIOS_RLS) /* asserts SCI on bit set */
/* start software smi timer on bit set */
#define   SWSMI_TMR_EN	(1 << SMI_SWSMI_TMR)
#define   APMC_EN	(1 << SMI_APMC) /* Writes to APM_CNT cause SMI# */
/* Write to SLP_EN in PM1_CNT asserts SMI# */
#define   SLP_SMI_EN	(1 << SMI_SLP)
#define   BIOS_EN	(1 << SMI_BIOS) /* Assert SMI# on GBL_RLS bit */
#define   EOS		(1 << SMI_EOS) /* End of SMI (deassert SMI#) */
#define   GBL_SMI_EN	(1 << SMI_GBL) /* Global SMI Enable */

/* SMI_EN Params for this platform to pass to enable_smi
 *
 * Enable SMI generation:
 *  - on APMC writes (io 0xb2)
 *  - on writes to SLP_EN (sleep states)
 *  - on writes to GBL_RLS (bios commands)
 *  - on eSPI events (does nothing on LPC systems)
 * No SMIs:
 *  - on microcontroller writes (io 0x62/0x66)
 *  - on TCO events
 */
#define   ENABLE_SMI_PARAMS \
	(ESPI_SMI_EN | APMC_EN | SLP_SMI_EN | GBL_SMI_EN | EOS | GPIO_EN)

#define SMI_STS			0x44
/* Bits for SMI status */
#define  ESPI_SMI_STS_BIT	28
#define  PMC_OCP_SMI_STS	27
#define  SPI_SMI_STS		26
#define  SPI_SSMI_STS		25
#define  SCC2_SMI_STS		21
#define  PCIE_SMI_STS		20
#define  SCS_SMI_STS		19
#define  HSMBUS_SMI_STS		18
#define  XHCI_SMI_STS		17
#define  SMBUS_SMI_STS		16
#define  SERIRQ_SMI_STS		15
#define  PERIODIC_SMI_STS	14
#define  TCO_SMI_STS		13
#define  MC_SMI_STS		12
#define  GPIO_UNLOCK_SMI_STS	11
#define  GPIO_SMI_STS		10
#define  FAKE_PM1_SMI_STS	8
#define  SWSMI_TMR_SMI_STS	6
#define  APM_SMI_STS		5
#define  SLP_SMI_STS		4
#define  LEGACY_USB_SMI_STS	3
#define  BIOS_SMI_STS		2

#define GPE_CNTL		0x50
#define DEVACT_STS		0x4c
#define TCO_STS			0x64
#define   TCO_TIMEOUT		(1 << 3)
#define TCO1_CNT		0x68
#define   TCO_TMR_HLT		(1 << 11)

#define GPE0_REG_MAX		4
#define GPE0_REG_SIZE		32
#define GPE0_STS(x)		(0x20 + (x * 4))
#define  GPE0_A			0
#define  GPE0_B			1
#define  GPE0_C			2
#define  GPE0_D			3
#define GPE_STD			GPE0_A
#define   ESPI_STS              (1 << 20) /* This bit is present in GLK */
#define   SATA_PME_STS		(1 << 17)
#define   SMB_WAK_STS		(1 << 16)
#define   AVS_PME_STS		(1 << 14)
#define   XHCI_PME_STS		(1 << 13)
#define   XDCI_PME_STS		(1 << 12)
#define   CSE_PME_STS		(1 << 11)
#define   BATLOW_STS		(1 << 10)
#define   PCIE_GPE_STS		(1 << 9)
#define   SWGPE_STS		(1 << 2)
#define GPE0_EN(x)		(0x30 + (x * 4))
#define   ESPI_EN		(1 << 20) /* This bit is present in GLK */
#define   SATA_PME_EN		(1 << 17)
#define   SMB_WAK_EN		(1 << 16)
#define   AVS_PME_EN		(1 << 14)
#define   PME_B0_EN		(1 << 13)
#define   XDCI_PME_EN		(1 << 12)
#define   CSE_PME_EN		(1 << 11)
#define   BATLOW_EN		(1 << 10)
#define   PCIE_GPE_EN		(1 << 9)
#define   SWGPE_EN		(1 << 2)

/* Memory mapped IO registers behind PMC_BASE_ADDRESS */
#define PRSTS			0x1000
#define GEN_PMCON1		0x1020
#define  COLD_BOOT_STS		(1 << 27)
#define  COLD_RESET_STS		(1 << 26)
#define  WARM_RESET_STS		(1 << 25)
#define  GLOBAL_RESET_STS	(1 << 24)
#define  SRS			(1 << 20)
#define  MS4V			(1 << 18)
#define  RPS			(1 << 2)
#define GEN_PMCON1_CLR1_BITS	(COLD_BOOT_STS | COLD_RESET_STS | \
				 WARM_RESET_STS | GLOBAL_RESET_STS | \
				 SRS | MS4V)
#define GEN_PMCON2		0x1024
#define GEN_PMCON3		0x1028
#       define SLP_S3_ASSERT_WIDTH_SHIFT	10
#       define SLP_S3_ASSERT_MASK	(0x3 << SLP_S3_ASSERT_WIDTH_SHIFT)
#       define SLP_S3_ASSERT_60_USEC	0x0
#       define SLP_S3_ASSERT_1_MSEC	0x1
#       define SLP_S3_ASSERT_50_MSEC	0x2
#       define SLP_S3_ASSERT_2_SEC	0x3
#define ETR			0x1048
#       define CF9_LOCK         (1 << 31)
#       define CF9_GLB_RST      (1 << 20)
#define GPIO_GPE_CFG		0x1050
#define  GPE0_DWX_MASK		0xf
#define GPE0_DW_SHIFT(x)	(4 + 4*(x))

#if IS_ENABLED(CONFIG_SOC_INTEL_GLK)
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

/* P-state configuration */
#define PSS_MAX_ENTRIES		8
#define PSS_RATIO_STEP		2
#define PSS_LATENCY_TRANSITION	10
#define PSS_LATENCY_BUSMASTER	10

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
} __packed;

void pch_log_state(void);

void enable_pm_timer_emulation(void);

#endif
