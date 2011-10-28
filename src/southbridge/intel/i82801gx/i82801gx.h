/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SOUTHBRIDGE_INTEL_I82801GX_I82801GX_H
#define SOUTHBRIDGE_INTEL_I82801GX_I82801GX_H
/*
 * It does not matter where we put the SMBus I/O base, as long as we
 * keep it consistent and don't interfere with other devices.  Stage2
 * will relocate this anyways.
 * Our solution is to have SMB initialization move the I/O to SMBUS_IO_BASE
 * again. But handling static BARs is a generic problem that should be
 * solved in the device allocator.
 */
#define SMBUS_IO_BASE		0x0400
/* TODO Make sure these don't get changed by stage2 */
#define DEFAULT_GPIOBASE	0x0480
#define DEFAULT_PMBASE		0x0500

#define HPET_ADDR		0xfed00000
#define DEFAULT_RCBA		0xfed1c000

#ifndef __ACPI__
#define DEBUG_PERIODIC_SMIS 0

#if !defined(__ASSEMBLER__) && !defined(__ROMCC__)
#if !defined(__PRE_RAM__)
#include "chip.h"
extern void i82801gx_enable(device_t dev);
#else
void enable_smbus(void);
int smbus_read_byte(unsigned device, unsigned address);
#endif
#endif

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

/* PCI Configuration Space (D30:F0): PCI2PCI */
#define PSTS	0x06
#define SMLT	0x1b
#define SECSTS	0x1e
#define INTR	0x3c
#define BCTRL	0x3e
#define   SBR	(1 << 6)
#define   SEE	(1 << 1)
#define   PERE	(1 << 0)

/* PCI Configuration Space (D31:F0): LPC */

#define SERIRQ_CNTL		0x64

#define GEN_PMCON_1		0xa0
#define GEN_PMCON_2		0xa2
#define GEN_PMCON_3		0xa4

/* GEN_PMCON_3 bits */
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)

#define PMBASE			0x40
#define ACPI_CNTL		0x44
#define BIOS_CNTL		0xDC
#define GPIO_BASE		0x48 /* LPC GPIO Base Address Register */
#define GPIO_CNTL		0x4C /* LPC GPIO Control Register */

#define PIRQA_ROUT		0x60
#define PIRQB_ROUT		0x61
#define PIRQC_ROUT		0x62
#define PIRQD_ROUT		0x63
#define PIRQE_ROUT		0x68
#define PIRQF_ROUT		0x69
#define PIRQG_ROUT		0x6A
#define PIRQH_ROUT		0x6B

#define LPC_IO_DEC		0x80 /* IO Decode Ranges Register */
#define LPC_EN			0x82 /* LPC IF Enables Register */

/* PCI Configuration Space (D31:F1): IDE */
#define INTR_LN			0x3c
#define IDE_TIM_PRI		0x40	/* IDE timings, primary */
#define   IDE_DECODE_ENABLE	(1 << 15)
#define   IDE_SITRE		(1 << 14)
#define   IDE_ISP_5_CLOCKS	(0 << 12)
#define   IDE_ISP_4_CLOCKS	(1 << 12)
#define   IDE_ISP_3_CLOCKS	(2 << 12)
#define   IDE_RCT_4_CLOCKS	(0 <<  8)
#define   IDE_RCT_3_CLOCKS	(1 <<  8)
#define   IDE_RCT_2_CLOCKS	(2 <<  8)
#define   IDE_RCT_1_CLOCKS	(3 <<  8)
#define   IDE_DTE1		(1 <<  7)
#define   IDE_PPE1		(1 <<  6)
#define   IDE_IE1		(1 <<  5)
#define   IDE_TIME1		(1 <<  4)
#define   IDE_DTE0		(1 <<  3)
#define   IDE_PPE0		(1 <<  2)
#define   IDE_IE0		(1 <<  1)
#define   IDE_TIME0		(1 <<  0)
#define IDE_TIM_SEC		0x42	/* IDE timings, secondary */

#define IDE_SDMA_CNT		0x48	/* Synchronous DMA control */
#define   IDE_SSDE1		(1 <<  3)
#define   IDE_SSDE0		(1 <<  2)
#define   IDE_PSDE1		(1 <<  1)
#define   IDE_PSDE0		(1 <<  0)

#define IDE_SDMA_TIM		0x4a

#define IDE_CONFIG		0x54	/* IDE I/O Configuration Register */
#define   SIG_MODE_SEC_NORMAL	(0 << 18)
#define   SIG_MODE_SEC_TRISTATE	(1 << 18)
#define   SIG_MODE_SEC_DRIVELOW	(2 << 18)
#define   SIG_MODE_PRI_NORMAL	(0 << 16)
#define   SIG_MODE_PRI_TRISTATE	(1 << 16)
#define   SIG_MODE_PRI_DRIVELOW	(2 << 16)
#define   FAST_SCB1		(1 << 15)
#define   FAST_SCB0		(1 << 14)
#define   FAST_PCB1		(1 << 13)
#define   FAST_PCB0		(1 << 12)
#define   SCB1			(1 <<  3)
#define   SCB0			(1 <<  2)
#define   PCB1			(1 <<  1)
#define   PCB0			(1 <<  0)

/* PCI Configuration Space (D31:F3): SMBus */
#define SMB_BASE		0x20
#define HOSTC			0x40

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

/* SMBus I/O bits. */
#define SMBHSTSTAT		0x0
#define SMBHSTCTL		0x2
#define SMBHSTCMD		0x3
#define SMBXMITADD		0x4
#define SMBHSTDAT0		0x5
#define SMBHSTDAT1		0x6
#define SMBBLKDAT		0x7
#define SMBTRNSADD		0x9
#define SMBSLVDATA		0xa
#define SMLINK_PIN_CTL		0xe
#define SMBUS_PIN_CTL		0xf

#define SMBUS_TIMEOUT		(10 * 1000 * 100)


/* Southbridge IO BARs */

#define GPIOBASE		0x48

#define PMBASE		0x40

/* Root Complex Register Block */
#define RCBA		0xf0

#define RCBA8(x) *((volatile u8 *)(DEFAULT_RCBA + x))
#define RCBA16(x) *((volatile u16 *)(DEFAULT_RCBA + x))
#define RCBA32(x) *((volatile u32 *)(DEFAULT_RCBA + x))

#define VCH		0x0000	/* 32bit */
#define VCAP1		0x0004	/* 32bit */
#define VCAP2		0x0008	/* 32bit */
#define PVC		0x000c	/* 16bit */
#define PVS		0x000e	/* 16bit */

#define V0CAP		0x0010	/* 32bit */
#define V0CTL		0x0014	/* 32bit */
#define V0STS		0x001a	/* 16bit */

#define V1CAP		0x001c	/* 32bit */
#define V1CTL		0x0020	/* 32bit */
#define V1STS		0x0026	/* 16bit */

#define RCTCL		0x0100	/* 32bit */
#define ESD		0x0104	/* 32bit */
#define ULD		0x0110	/* 32bit */
#define ULBA		0x0118	/* 64bit */

#define RP1D		0x0120	/* 32bit */
#define RP1BA		0x0128	/* 64bit */
#define RP2D		0x0130	/* 32bit */
#define RP2BA		0x0138	/* 64bit */
#define RP3D		0x0140	/* 32bit */
#define RP3BA		0x0148	/* 64bit */
#define RP4D		0x0150	/* 32bit */
#define RP4BA		0x0158	/* 64bit */
#define HDD		0x0160	/* 32bit */
#define HDBA		0x0168	/* 64bit */
#define RP5D		0x0170	/* 32bit */
#define RP5BA		0x0178	/* 64bit */
#define RP6D		0x0180	/* 32bit */
#define RP6BA		0x0188	/* 64bit */

#define ILCL		0x01a0	/* 32bit */
#define LCAP		0x01a4	/* 32bit */
#define LCTL		0x01a8	/* 16bit */
#define LSTS		0x01aa	/* 16bit */

#define RPC		0x0224	/* 32bit */
#define RPFN		0x0238	/* 32bit */

#define TRSR		0x1e00	/*  8bit */
#define TRCR		0x1e10	/* 64bit */
#define TWDR		0x1e18	/* 64bit */

#define IOTR0		0x1e80	/* 64bit */
#define IOTR1		0x1e88	/* 64bit */
#define IOTR2		0x1e90	/* 64bit */
#define IOTR3		0x1e98	/* 64bit */

#define TCTL		0x3000	/*  8bit */

#define D31IP		0x3100	/* 32bit */
#define D30IP		0x3104	/* 32bit */
#define D29IP		0x3108	/* 32bit */
#define D28IP		0x310c	/* 32bit */
#define D27IP		0x3110	/* 32bit */
#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define OIC		0x31ff	/*  8bit */

#define RC		0x3400	/* 32bit */
#define HPTC		0x3404	/* 32bit */
#define GCS		0x3410	/* 32bit */
#define BUC		0x3414	/* 32bit */
#define FD		0x3418	/* 32bit */
#define CG		0x341c	/* 32bit */

/* Function Disable (FD) register values.
 * Setting a bit disables the corresponding
 * feature.
 * Not all features might be disabled on
 * all chipsets. Esp. ICH-7U is picky.
 */
#define FD_PCIE6	(1 << 21)
#define FD_PCIE5	(1 << 20)
#define FD_PCIE4	(1 << 19)
#define FD_PCIE3	(1 << 18)
#define FD_PCIE2	(1 << 17)
#define FD_PCIE1	(1 << 16)
#define FD_EHCI		(1 << 15)
#define FD_LPCB		(1 << 14)

/* UHCI must be disabled from 4 downwards.
 * If UHCI controllers get disabled, EHCI
 * must know about it, too! */
#define FD_UHCI4	(1 << 11)
#define FD_UHCI34	(1 << 10) | FD_UHCI4
#define FD_UHCI234	(1 <<  9) | FD_UHCI3
#define FD_UHCI1234	(1 <<  8) | FD_UHCI2

#define FD_INTLAN	(1 <<  7)
#define FD_ACMOD	(1 <<  6)
#define FD_ACAUD	(1 <<  5)
#define FD_HDAUD	(1 <<  4)
#define FD_SMBUS	(1 <<  3)
#define FD_SATA		(1 <<  2)
#define FD_PATA		(1 <<  1)

/* ICH7 GPIOBASE */
#define GPIO_USE_SEL	0x00
#define GP_IO_SEL	0x04
#define GP_LVL		0x0c
#define GPO_BLINK	0x18
#define GPI_INV		0x2c
#define GPIO_USE_SEL2	0x30
#define GP_IO_SEL2	0x34
#define GP_LVL2		0x38

/* ICH7 PMBASE */
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
#define   USB4_STS	(1 << 14)
#define   PME_B0_STS	(1 << 13)
#define   USB3_STS	(1 << 12)
#define   PME_STS	(1 << 11)
#define   BATLOW_STS	(1 << 10)
#define   PCI_EXP_STS	(1 << 9)
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

#define SKPAD_ACPI_S3_MAGIC	0xcafed00d
#define SKPAD_NORMAL_BOOT_MAGIC	0xcafebabe
#endif /* __ACPI__ */
#endif				/* SOUTHBRIDGE_INTEL_I82801GX_I82801GX_H */
