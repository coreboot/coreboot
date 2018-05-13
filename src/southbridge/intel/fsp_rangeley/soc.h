/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors.  All rights reserved.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#ifndef SOUTHBRIDGE_INTEL_RANGELEY_SOC_H
#define SOUTHBRIDGE_INTEL_RANGELEY_SOC_H

#include <arch/acpi.h>

/* SOC types */
#define SOC_TYPE_RANGELEY	0x1F

/*
 * It does not matter where we put the SMBus I/O base, as long as we
 * keep it consistent and don't interfere with other devices.  Stage2
 * will relocate this anyways.
 * Our solution is to have SMB initialization move the I/O to SMBUS_IO_BASE
 * again. But handling static BARs is a generic problem that should be
 * solved in the device allocator.
 */

/* Southbridge internal device IO BARs (Set to match FSP settings) */
#define SMBUS_IO_BASE		0xefa0
#define SMBUS_SLAVE_ADDR	0x24
#define DEFAULT_GPIOBASE	0x0500
#define DEFAULT_ABASE		0x0400

/* Southbridge internal device MEM BARs (Set to match FSP settings) */
#define DEFAULT_IBASE		0xfed08000
#define DEFAULT_PBASE		0xfed03000
#ifndef __ACPI__
#define DEFAULT_RCBA		((u8 *)0xfed1c000)
#else
#define DEFAULT_RCBA		0xfed1c000
#endif

#ifndef __ACPI__
#define DEBUG_PERIODIC_SMIS 0

#if defined (__SMM__) && !defined(__ASSEMBLER__)
void intel_soc_finalize_smm(void);
#endif

#if !defined(__ASSEMBLER__) && !defined(__ROMCC__)
#if !defined(__PRE_RAM__) && !defined(__SMM__)
#include "chip.h"
int soc_silicon_revision(void);
int soc_silicon_type(void);
int soc_silicon_supported(int type, int rev);
void soc_enable(struct device *dev);

#include <arch/acpi.h>
void acpi_fill_in_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt);

#if IS_ENABLED(CONFIG_ELOG)
void soc_log_state(void);
#endif
#else
void enable_smbus(void);
void enable_usb_bar(void);
int smbus_read_byte(unsigned device, unsigned address);
int early_spi_read(u32 offset, u32 size, u8 *buffer);
void rangeley_sb_early_initialization(void);
#endif
#endif

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

#define SOC_EHCI1_DEV		PCI_DEV(0, 0x1d, 0)
#define PCIE_DEV_SLOT0	1
#define PCIE_DEV_SLOT1	2
#define PCIE_DEV_SLOT2	3
#define PCIE_DEV_SLOT3	4

/* PCI Configuration Space (D31:F0): LPC */
#define SOC_LPC_DEV		PCI_DEV(0, 0x1f, 0)
#define SOC_LPC_DEVFN 0, PCI_DEVFN(0x1f,0)


/* Southbridge IO BARs */
#define ABASE		0x40 /* IO BAR */
#define PBASE		0x44 /* MEM BAR */
#define GBASE		0x48 /* IO BAR */
#define IOBASE		0x4C /* MEM BAR */
#define IBASE		0x50 /* MEM BAR */
#define SBASE		0x54 /* MEM BAR */
#define MPBASE		0x58 /* MEM BAR */
#define  SET_BAR_ENABLE	0x02

/* Rangeley ILB defines */
#define ILB_ACTL	0
#define ILB_PIRQA_ROUT	0x8
#define ILB_PIRQB_ROUT	0x9
#define ILB_PIRQC_ROUT	0xA
#define ILB_PIRQD_ROUT	0xB
#define ILB_PIRQE_ROUT	0xC
#define ILB_PIRQF_ROUT	0xD
#define ILB_PIRQG_ROUT	0xE
#define ILB_PIRQH_ROUT	0xF
#define ILB_SERIRQ_CNTL	0x10
#define ILB_IR00 0x20
#define ILB_IR01 0x22
#define ILB_IR02 0x24
#define ILB_IR03 0x26
#define ILB_IR04 0x28
#define ILB_IR05 0x2A
#define ILB_IR06 0x2C
#define ILB_IR07 0x2E
#define ILB_IR08 0x30
#define ILB_IR09 0x32
#define ILB_IR10 0x34
#define ILB_IR11 0x36
#define ILB_IR12 0x38
#define ILB_IR13 0x3A
#define ILB_IR14 0x3C
#define ILB_IR15 0x3E
#define ILB_IR16 0x40
#define ILB_IR17 0x42
#define ILB_IR18 0x44
#define ILB_IR19 0x46
#define ILB_IR20 0x48
#define ILB_IR21 0x4A
#define ILB_IR22 0x4C
#define ILB_IR23 0x4E
#define ILB_IR24 0x50
#define ILB_IR25 0x52
#define ILB_IR26 0x54
#define ILB_IR27 0x56
#define ILB_IR28 0x58
#define ILB_IR29 0x5A
#define ILB_IR30 0x5C
#define ILB_IR31 0x5E
#define ILB_OIC	0x60

/* PCI Configuration Space (D31:F2/5) */
#define SOC_SATA_DEV		PCI_DEV(0, 0x17, 0)
#define SOC_SATA2_DEV		PCI_DEV(0, 0x18, 0)

#define SATA_SIRI		0xa0 /* SATA Indexed Register Index */
#define SATA_SIRD		0xa4 /* SATA Indexed Register Data */
#define SATA_SP			0xd0 /* Scratchpad */
#define SATA_MAP		0x90
#define SATA_PSC		0x92

/* SATA IOBP Registers */
#define SATA_IOBP_SP0G3IR	0xea000151
#define SATA_IOBP_SP1G3IR	0xea000051

/* PCI Configuration Space (D31:F3): SMBus */
#define SOC_SMBUS_DEV		PCI_DEV(0, 0x1f, 3)
#define SMB_BASE		0x20
#define HOSTC			0x40
#define SMB_RCV_SLVA		0x09

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

/* Root Complex Register Block */
#define RCBA		0xf0
#define  RCBA_ENABLE 0x01

#define RCBA8(x) *((volatile u8 *)(DEFAULT_RCBA + x))
#define RCBA16(x) *((volatile u16 *)(DEFAULT_RCBA + x))
#define RCBA32(x) *((volatile u32 *)(DEFAULT_RCBA + x))

#define RCBA_AND_OR(bits, x, and, or) \
        RCBA##bits(x) = ((RCBA##bits(x) & (and)) | (or))
#define RCBA8_AND_OR(x, and, or)  RCBA_AND_OR(8, x, and, or)
#define RCBA16_AND_OR(x, and, or) RCBA_AND_OR(16, x, and, or)
#define RCBA32_AND_OR(x, and, or) RCBA_AND_OR(32, x, and, or)
#define RCBA32_OR(x, or) RCBA_AND_OR(32, x, ~0UL, or)

/* Root Port configuration space hide */
#define RPFN_HIDE(port)         (1 << (((port) * 4) + 3))
/* Get the function number assigned to a Root Port */
#define RPFN_FNGET(reg,port)    (((reg) >> ((port) * 4)) & 7)
/* Set the function number for a Root Port */
#define RPFN_FNSET(port,func)   (((func) & 7) << ((port) * 4))
/* Root Port function number mask */
#define RPFN_FNMASK(port)       (7 << ((port) * 4))


#define NOINT		0
#define INTA		1
#define INTB		2
#define INTC		3
#define INTD		4

#define DIR_IDR		12	/* Interrupt D Pin Offset */
#define DIR_ICR		8	/* Interrupt C Pin Offset */
#define DIR_IBR		4	/* Interrupt B Pin Offset */
#define DIR_IAR		0	/* Interrupt A Pin Offset */

#define PIRQA		0
#define PIRQB		1
#define PIRQC		2
#define PIRQD		3
#define PIRQE		4
#define PIRQF		5
#define PIRQG		6
#define PIRQH		7

/* IO Buffer Programming */
#define IOBPIRI		0x2330
#define IOBPD		0x2334
#define IOBPS		0x2338
#define  IOBPS_RW_BX    ((1 << 9)|(1 << 10))
#define  IOBPS_WRITE_AX	((1 << 9)|(1 << 10))
#define  IOBPS_READ_AX	((1 << 8)|(1 << 9)|(1 << 10))



#define DIR_ROUTE(x,a,b,c,d) \
  RCBA32(x) = (((d) << DIR_IDR) | ((c) << DIR_ICR) | \
               ((b) << DIR_IBR) | ((a) << DIR_IAR))

/* PBASE Registers */
#define PMC_CFG	0x08
#define  SPS					(1 << 5)
#define  NO_REBOOT		(1 << 4)
#define  SX_ENT_TO_EN	(1 << 3)
#define  TIMING_T581	(1 << 0)

#define GEN_PMCON1	0x20
#	define DISB				(1 << 23)
#	define MEM_SR			(1 << 21)
#	define SRS				(1 << 20)
#	define CTS				(1 << 19)
#	define MS4V				(1 << 18)
#	define PWR_FLR			(1 << 16)
#	define PME_B0_S5_DIS	(1 << 15)
#	define SUS_PWR_FLR		(1 << 14)
#	define WOL_EN_OVRD		(1 << 13)
#	define DIS_SLP_X_STRCH_SUS_UP (1 << 12)
#	define GEN_RST_STS		(1 <<  9)
#	define RPS				(1 <<  2)
#	define AFTERG3_EN		(1 <<  0)

/* Function Disable PBASE + 0x34 */
#define PBASE_FUNC_DIS 0x34
#define PBASE_DISABLE_QUICKASSIST	(1 << 0)
#define PBASE_DISABLE_GBE(x)	(1 << (12 + x))
#define PBASE_DISABLE_SATA2	(1 << 22)
#define PBASE_DISABLE_EHCI	(1 << 23)
#define PBASE_DISABLE_SATA3	(1 << 23)

/* GPIOBASE */
#define GPIO_SC_USE_SEL  0x00
#define GPIO_SC_IO_SEL   0x04
#define GPIO_SC_GP_LVL   0x08
#define GPIO_SC_TPE      0x0c
#define GPIO_SC_TNE      0x10
#define GPIO_SC_TS       0x14
#define GPIO_SUS_USE_SEL 0x80
#define GPIO_SUS_IO_SEL  0x84
#define GPIO_SUS_GP_LVL  0x88
#define GPIO_SUS_TPE     0x8c
#define GPIO_SUS_TNE     0x90
#define GPIO_SUS_TS      0x94
#define GPIO_SUS_WE      0x98

/* IOBASE */
#define CFIO_PAD_CONF0  0x00
#define CFIO_PAD_CONF1  0x04
#define CFIO_PAD_VAL    0x08
#define CFIO_PAD_DFT    0x0C

/* ACPI BASE */
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
#define LV3		0x15
#define LV4		0x16
#define PM2_CNT		0x50 // mobile only
#define GPE0_STS	0x20
#define   PME_B0_STS	(1 << 13)
#define   PME_STS	(1 << 11)
#define   BATLOW_STS	(1 << 10)
#define   PCI_EXP_STS	(1 << 9)
#define   RI_STS	(1 << 8)
#define   SMB_WAK_STS	(1 << 7)
#define   TCOSCI_STS	(1 << 6)
#define   SWGPE_STS	(1 << 2)
#define   HOT_PLUG_STS	(1 << 1)
#define GPE0_EN		0x28
#define   PME_B0_EN	(1 << 13)
#define   PME_EN	(1 << 11)
#define   TCOSCI_EN	(1 << 6)
#define SMI_EN		0x30
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
#define PM2A_CNT_BLK	0x50
#define SS_CNT		0x50
#define C3_RES		0x54
#define TCO1_STS	0x64
#define   DMISCI_STS	(1 << 9)
#define TCO2_STS	0x66
#define TCO1_CNT	0x68
#define  TCO_TMR_HALT	(1 << 11)
#define  TCO_LOCK			(1 << 12)

/*
 * SPI Opcode Menu setup for SPIBAR lockdown
 * should support most common flash chips.
 */

#define SPI_OPMENU_0 0x01 /* WRSR: Write Status Register */
#define SPI_OPTYPE_0 0x01 /* Write, no address */

#define SPI_OPMENU_1 0x02 /* BYPR: Byte Program */
#define SPI_OPTYPE_1 0x03 /* Write, address required */

#define SPI_OPMENU_2 0x03 /* READ: Read Data */
#define SPI_OPTYPE_2 0x02 /* Read, address required */

#define SPI_OPMENU_3 0x05 /* RDSR: Read Status Register */
#define SPI_OPTYPE_3 0x00 /* Read, no address */

#define SPI_OPMENU_4 0x20 /* SE20: Sector Erase 0x20 */
#define SPI_OPTYPE_4 0x03 /* Write, address required */

#define SPI_OPMENU_5 0x9f /* RDID: Read ID */
#define SPI_OPTYPE_5 0x00 /* Read, no address */

#define SPI_OPMENU_6 0xd8 /* BED8: Block Erase 0xd8 */
#define SPI_OPTYPE_6 0x03 /* Write, address required */

#define SPI_OPMENU_7 0x0b /* FAST: Fast Read */
#define SPI_OPTYPE_7 0x02 /* Read, address required */

#define SPI_OPMENU_UPPER ((SPI_OPMENU_7 << 24) | (SPI_OPMENU_6 << 16) | \
			  (SPI_OPMENU_5 << 8) | SPI_OPMENU_4)
#define SPI_OPMENU_LOWER ((SPI_OPMENU_3 << 24) | (SPI_OPMENU_2 << 16) | \
			  (SPI_OPMENU_1 << 8) | SPI_OPMENU_0)

#define SPI_OPTYPE ((SPI_OPTYPE_7 << 14) | (SPI_OPTYPE_6 << 12) | \
		    (SPI_OPTYPE_5 << 10) | (SPI_OPTYPE_4 << 8) |  \
		    (SPI_OPTYPE_3 << 6) | (SPI_OPTYPE_2 << 4) |	  \
		    (SPI_OPTYPE_1 << 2) | (SPI_OPTYPE_0))

#define SPI_OPPREFIX ((0x50 << 8) | 0x06) /* EWSR and WREN */

#define SPIBAR_HSFS                 0x04   /* SPI hardware sequence status */
#define  SPIBAR_HSFS_SCIP           (1 << 5) /* SPI Cycle In Progress */
#define  SPIBAR_HSFS_AEL            (1 << 2) /* SPI Access Error Log */
#define  SPIBAR_HSFS_FCERR          (1 << 1) /* SPI Flash Cycle Error */
#define  SPIBAR_HSFS_FDONE          (1 << 0) /* SPI Flash Cycle Done */
#define SPIBAR_HSFC                 0x06   /* SPI hardware sequence control */
#define  SPIBAR_HSFC_BYTE_COUNT(c)  (((c - 1) & 0x3f) << 8)
#define  SPIBAR_HSFC_CYCLE_READ     (0 << 1) /* Read cycle */
#define  SPIBAR_HSFC_CYCLE_WRITE    (2 << 1) /* Write cycle */
#define  SPIBAR_HSFC_CYCLE_ERASE    (3 << 1) /* Erase cycle */
#define  SPIBAR_HSFC_GO             (1 << 0) /* GO: start SPI transaction */
#define SPIBAR_FADDR                0x08   /* SPI flash address */
#define SPIBAR_FDATA(n)             (0x3810 + (4 * n)) /* SPI flash data */

/* HPET Registers - Base is set in hardware to 0xFED00000 */
#define HPET_GCID		0xFED00000	/* General Capabilities and ID */
#define HPET_GCFG		0xFED00010	/* General Configuration */
#define HPET_GIS		0xFED00020	/* General Interrupt Status */
#define HPET_MCV		0xFED000F0	/* Main Counter Value */
#define HPET_T0C		0xFED00100	/* Timer 0 Configuration and Capabilities */
#define HPET_T0CV_L	0xFED00108	/* Lower Timer 0 Comparator Value */
#define HPET_T0CV_U	0xFED0010C	/* Upper Timer 0 Comparator Value */
#define HPET_T1C		0xFED00120	/* Timer 1 Configuration and Capabilities */
#define HPET_T1CV		0xFED00128	/* Timer 1 Comparator Value */
#define HPET_T2C		0xFED00140	/* Timer 2 Configuration and Capabilities */
#define HPET_T2CV		0xFED00148	/* Timer 2 Comparator Value */


#endif /* __ACPI__ */
#endif /* SOUTHBRIDGE_INTEL_RANGELEY_SOC_H */
