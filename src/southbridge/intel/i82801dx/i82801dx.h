/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Ron G. Minnich
 * Copyright (C) 2004 Eric Biederman
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* the problem: we have 82801dbm support in fb1, and 82801er in fb2.
 * fb1 code is what we want, fb2 structure is needed however.
 * so we need to get fb1 code for 82801dbm into fb2 structure.
 */
/* What I did: took the 80801er stuff from fb2, verify it against the
 * db stuff in fb1, and made sure it was right.
 */

#ifndef I82801DX_H
#define I82801DX_H

#if !defined(__ASSEMBLER__)
#if !defined(__PRE_RAM__)
#include "chip.h"
extern void i82801dx_enable(device_t dev);
#else
void enable_smbus(void);
int smbus_read_byte(unsigned device, unsigned address);
#endif
#endif

#define DEBUG_PERIODIC_SMIS 0

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

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
#define   PMBASE_ADDR	0x0400
#define   DEFAULT_PMBASE PMBASE_ADDR
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

#define RTC_FAILED      (1 <<2)


#define SMBUS_IO_BASE 0x1000

#define SMBHSTSTAT 0x0
#define SMBHSTCTL  0x2
#define SMBHSTCMD  0x3
#define SMBXMITADD 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBBLKDAT  0x7
#define SMBTRNSADD 0x9
#define SMBSLVDATA 0xa
#define SMLINK_PIN_CTL 0xe
#define SMBUS_PIN_CTL  0xf

/* Between 1-10 seconds, We should never timeout normally
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT (100*1000)

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
#define   SLP_EN	(1 << 13)
#define   SLP_TYP	(7 << 10)
#define   GBL_RLS	(1 << 2)
#define   BM_RLD	(1 << 1)
#define   SCI_EN	(1 << 0)
#define PM1_TMR		0x08
#define PROC_CNT	0x10
#define LV2		0x14
#define LV3		0x15
#define LV4		0x16
#define PM2_CNT		0x20 // mobile only
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
#define C3_RES		0x54

#define TCOBASE		0x60 /* TCO Base Address Register */
#define TCO1_CNT	0x08 /* TCO1 Control Register */

#define GEN_PMCON_1		0xa0
#define GEN_PMCON_2		0xa2
#define GEN_PMCON_3		0xa4

/* GEN_PMCON_3 bits */
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)

#endif /* I82801DX_H */
