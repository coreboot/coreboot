/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef I82801DX_H
#define I82801DX_H

#include <acpi/acpi.h>
#include <device/device.h>

void i82801dx_enable(struct device *dev);
void i82801dx_early_init(void);
void i82801dx_lpc_setup(void);

#define DEBUG_PERIODIC_SMIS 0

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

/*
 * 000 = Non-combined. P0 is primary master. P1 is secondary master.
 * 001 = Non-combined. P0 is secondary master. P1 is primary master.
 * 100 = Combined. P0 is primary master. P1 is primary slave. IDE is secondary;
 *       Primary IDE channel disabled.
 * 101 = Combined. P0 is primary slave. P1 is primary master. IDE is secondary.
 * 110 = Combined. IDE is primary. P0 is secondary master. P1 is secondary
 *       slave; Secondary IDE channel disabled.
 * 111 = Combined. IDE is primary. P0 is secondary slave. P1 is secondary master.
 */
/* PCI Configuration Space (D31:F1) */
#define IDE_TIM_PRI		0x40	/* IDE timings, primary */
#define IDE_TIM_SEC		0x42	/* IDE timings, secondary */

/* IDE_TIM bits */
#define IDE_DECODE_ENABLE	(1 << 15)

#define PCI_DMA_CFG     0x90
#define SERIRQ_CNTL     0x64
#define GEN_CNTL        0xd0
#define GEN_STS         0xd4
#define RTC_CONF        0xd8
#define GEN_PMCON_3     0xa4

#define PCICMD          0x04
#define PMBASE          0x40
#define   DEFAULT_PMBASE 0x0400
#define ACPI_CNTL       0x44
#define   ACPI_EN	(1 << 4)
#define BIOS_CNTL       0x4E
#define GPIO_BASE       0x58
#define GPIO_CNTL       0x5C
#define   GPIOBASE_ADDR 0x0500
#define PIRQA_ROUT	0x60
#define PIRQB_ROUT	0x61
#define PIRQC_ROUT	0x62
#define PIRQD_ROUT	0x63
#define PIRQE_ROUT	0x68
#define PIRQF_ROUT	0x69
#define PIRQG_ROUT	0x6A
#define PIRQH_ROUT	0x6B
#define COM_DEC         0xE0
#define GEN1_DEC        0xE4
#define LPC_EN          0xE6
#define FUNC_DIS        0xF2

/* 1e f0 244e */

#define CMD             0x04
#define SBUS_NUM        0x19
#define SUB_BUS_NUM     0x1A
#define SMLT            0x1B
#define IOBASE          0x1C
#define IOLIM           0x1D
#define MEMBASE         0x20
#define MEMLIM          0x22
#define CNF             0x50
#define MTT             0x70
#define PCI_MAST_STS    0x82

#define RTC_FAILED      (1 << 2)

#define PM1_STS		0x00
#define   WAK_STS	(1 << 15)
#define   PCIEXPWAK_STS	(1 << 14)
#define   PRBTNOR_STS	(1 << 11)
#define   RTC_STS	(1 << 10)
#define   PWRBTN_STS	(1 << 8)
#define   GBL_STS	(1 << 5)
#define   BM_STS	(1 << 4)
#define   TMROF_STS	(1 << 0)
#define PM1_EN		0x02
#define   PCIEXPWAK_DIS	(1 << 14)
#define   RTC_EN	(1 << 10)
#define   PWRBTN_EN	(1 << 8)
#define   GBL_EN	(1 << 5)
#define   TMROF_EN	(1 << 0)
#define PM1_CNT		0x04
#define   GBL_RLS	(1 << 2)
#define   BM_RLD	(1 << 1)
#define   SCI_EN	(1 << 0)
#define PM1_TMR		0x08
#define PROC_CNT	0x10
#define LV2		0x14
#define GPE0_STS	0x28
#define   PME_B0_STS	(1 << 13)
#define   USB3_STS	(1 << 12)
#define   PME_STS	(1 << 11)
#define   BATLOW_STS	(1 << 10)
#define   GST_STS	(1 << 9)
#define   RI_STS	(1 << 8)
#define   SMB_WAK_STS	(1 << 7)
#define   TCOSCI_STS	(1 << 6)
#define   AC97_STS	(1 << 5)
#define   USB2_STS	(1 << 4)
#define   USB1_STS	(1 << 3)
#define   SWGPE_STS	(1 << 2)
#define   HOT_PLUG_STS	(1 << 1)
#define   THRM_STS	(1 << 0)
#define GPE0_EN		0x2c
#define   PME_B0_EN	(1 << 13)
#define   PME_EN	(1 << 11)
#define SMI_EN		0x30
#define   EL_SMI_EN	 (1 << 25) // Intel Quick Resume Technology
#define   INTEL_USB2_EN	 (1 << 18) // Intel-Specific USB2 SMI logic
#define   LEGACY_USB2_EN (1 << 17) // Legacy USB2 SMI logic
#define   PERIODIC_EN	 (1 << 14) // SMI on PERIODIC_STS in SMI_STS
#define   TCO_EN	 (1 << 13) // Enable TCO Logic (BIOSWE et al)
#define   MCSMI_EN	 (1 << 11) // Trap microcontroller range access
#define   BIOS_RLS	 (1 <<  7) // asserts SCI on bit set
#define   SWSMI_TMR_EN	 (1 <<  6) // start software smi timer on bit set
#define   APMC_EN	 (1 <<  5) // Writes to APM_CNT cause SMI#
#define   SLP_SMI_EN	 (1 <<  4) // Write to SLP_EN in PM1_CNT asserts SMI#
#define   LEGACY_USB_EN  (1 <<  3) // Legacy USB circuit SMI logic
#define   BIOS_EN	 (1 <<  2) // Assert SMI# on setting GBL_RLS bit
#define   EOS		 (1 <<  1) // End of SMI (deassert SMI#)
#define   GBL_SMI_EN	 (1 <<  0) // SMI# generation at all?
#define SMI_STS		0x34
#define ALT_GP_SMI_EN	0x38
#define ALT_GP_SMI_STS	0x3a
#define GPE_CNTL	0x42
#define DEVACT_STS	0x44
#define SS_CNT		0x50

#if CONFIG(TCO_SPACE_NOT_YET_SPLIT)
/* TCO1 Control Register */
#define TCO1_CNT	0x68
#endif

#define GEN_PMCON_1		0xa0
#define GEN_PMCON_2		0xa2
#define GEN_PMCON_3		0xa4

/* GEN_PMCON_3 bits */
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)

#endif /* I82801DX_H */
