/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _CS5536_H
#define _CS5536_H

#define Cx5536_ID	 (	0x208F1022)

/* SouthBridge Equates */
#define CS5536_GLINK_PORT_NUM	0x02	/* port of the SouthBridge */
#define NB_PCI			((2	<< 29) + (4 << 26))	/* NB GLPCI is in the same location on all Geodes. */
#define MSR_SB			((CS5536_GLINK_PORT_NUM << 23) + NB_PCI)	/* address to the SouthBridge */
#define SB_SHIFT		20	/* 29 -> 26 -> 23 -> 20...... When making a SB address uses this shift. */

#define CS5536_DEV_NUM	0x0F	/* default PCI device number for CS5536 */
#define SMBUS_IO_BASE	0x6000
#define GPIO_IO_BASE	0x6100
#define MFGPT_IO_BASE	0x6200
#define ACPI_IO_BASE	0x9C00
#define PMS_IO_BASE	0x9D00

#define CS5535_IDSEL	0x02000000	/* IDSEL = AD25, device #15 */
#define CHIPSET_DEV_NUM	15
#define IDSEL_BASE	11		/* bit 11 = device 1 */

/* Cs5536 as follows. */
/*	SB_GLIU */
/*	port0 - GLIU */
/*	port1 - GLPCI */
/*	port2 - USB Controller #2 */
/*	port3 - ATA-5 Controller */
/*	port4 - MDD */
/*	port5 - AC97 */
/*	port6 - USB Controller #1 */
/*	port7 - GLCP */

#define MSR_SB_GLIU		((9 << 14) + MSR_SB)		/* 51024xxx or 510*xxxx - fake out just like GL0 on CPU. */
#define MSR_SB_GLPCI		(MSR_SB)			/* 5100xxxx - don't go to the GLIU */
#define MSR_SB_USB2		((2 << SB_SHIFT) + MSR_SB)	/* 5120xxxx */
#define MSR_SB_ATA		((3 << SB_SHIFT) + MSR_SB)	/* 5130xxxx */
#define MSR_SB_MDD		((4 << SB_SHIFT) + MSR_SB)	/* 5140xxxx, a.k.a. DIVIL = Diverse Integrated Logic device */
#define MSR_SB_AC97		((5 << SB_SHIFT) + MSR_SB)	/* 5150xxxx */
#define MSR_SB_USB1		((6 << SB_SHIFT) + MSR_SB)	/* 5160xxxx */
#define MSR_SB_GLCP		((7 << SB_SHIFT) + MSR_SB)	/* 5170xxxx */

/* GLIU */
#define GLIU_SB_GLD_MSR_CAP	(MSR_SB_GLIU + 0x00)
#define GLIU_SB_GLD_MSR_CONF	(MSR_SB_GLIU + 0x01)
#define GLIU_SB_GLD_MSR_PM	(MSR_SB_GLIU + 0x04)

/* USB1 */
#define USB1_SB_GLD_MSR_CAP	(MSR_SB_USB1 + 0x00)
#define USB1_SB_GLD_MSR_CONF	(MSR_SB_USB1 + 0x01)
#define USB1_SB_GLD_MSR_PM	(MSR_SB_USB1 + 0x04)

/* USB2 */
#define USB2_SB_GLD_MSR_CAP		(MSR_SB_USB2 + 0x00)
#define USB2_SB_GLD_MSR_CONF		(MSR_SB_USB2 + 0x01)
#define	USB2_UPPER_SSDEN_SET			(1 << 3 )	/* Bit 35 */
#define USB2_SB_GLD_MSR_PM		(MSR_SB_USB2 + 0x04)
#define USB2_SB_GLD_MSR_DIAG		(MSR_SB_USB2 + 0x05)
#define USB2_SB_GLD_MSR_OHCI_BASE	(MSR_SB_USB2 + 0x08)
#define USB2_SB_GLD_MSR_EHCI_BASE	(MSR_SB_USB2 + 0x09)
#define USB2_SB_GLD_MSR_DEVCTL_BASE	(MSR_SB_USB2 + 0x0A)
#define USB2_SB_GLD_MSR_UOC_BASE	(MSR_SB_USB2 + 0x0B)	/* Option controller base */

/* ATA */
#define ATA_SB_GLD_MSR_CAP		(MSR_SB_ATA + 0x00)
#define ATA_SB_GLD_MSR_CONF		(MSR_SB_ATA + 0x01)
#define ATA_SB_GLD_MSR_ERR		(MSR_SB_ATA + 0x03)
#define ATA_SB_GLD_MSR_PM		(MSR_SB_ATA + 0x04)
#define ATA_SB_IDE_CFG			(MSR_SB_ATA + 0x10)

/* AC97 */
#define AC97_SB_GLD_MSR_CAP		(MSR_SB_AC97 + 0x00)
#define AC97_SB_GLD_MSR_CONF		(MSR_SB_AC97 + 0x01)
#define AC97_SB_GLD_MSR_PM		(MSR_SB_AC97 + 0x04)

/* GLPCI */
#define GLPCI_SB_GLD_MSR_CAP		(MSR_SB_GLPCI + 0x00)
#define GLPCI_SB_GLD_MSR_CONF		(MSR_SB_GLPCI + 0x01)
#define GLPCI_SB_GLD_MSR_PM		(MSR_SB_GLPCI + 0x04)
#define GLPCI_SB_CTRL			(MSR_SB_GLPCI + 0x10)
#define		GLPCI_CRTL_PPIDE_SET		(1 << 17)

/* GLCP */
#define GLCP_SB_GLD_MSR_CAP		(MSR_SB_GLCP + 0x00)
#define GLCP_SB_GLD_MSR_CONF		(MSR_SB_GLCP + 0x01)
#define GLCP_SB_GLD_MSR_PM		(MSR_SB_GLCP + 0x04)
#define GLCP_SB_CLKOFF			(MSR_SB_GLCP + 0x10)

/* MDD */
#define MDD_SB_GLD_MSR_CAP	(MSR_SB_MDD + 0x00)
#define MDD_SB_GLD_MSR_CONF	(MSR_SB_MDD + 0x01)
#define MDD_SB_GLD_MSR_PM	(MSR_SB_MDD + 0x04)
#define LBAR_EN				(0x01)
#define IO_MASK				(0x1f)
#define MEM_MASK			(0x0FFFFF)
#define MDD_LBAR_IRQ		(MSR_SB_MDD + 0x08)
#define MDD_LBAR_KEL1		(MSR_SB_MDD + 0x09)
#define MDD_LBAR_KEL2		(MSR_SB_MDD + 0x0A)
#define MDD_LBAR_SMB		(MSR_SB_MDD + 0x0B)
#define MDD_LBAR_GPIO		(MSR_SB_MDD + 0x0C)
#define MDD_LBAR_MFGPT		(MSR_SB_MDD + 0x0D)
#define MDD_LBAR_ACPI		(MSR_SB_MDD + 0x0E)
#define MDD_LBAR_PMS		(MSR_SB_MDD + 0x0F)

#define MDD_LBAR_FLSH0		(MSR_SB_MDD + 0x10)
#define MDD_LBAR_FLSH1		(MSR_SB_MDD + 0x11)
#define MDD_LBAR_FLSH2		(MSR_SB_MDD + 0x12)
#define MDD_LBAR_FLSH3		(MSR_SB_MDD + 0x13)
#define MDD_LEG_IO		(MSR_SB_MDD + 0x14)
#define MDD_PIN_OPT		(MSR_SB_MDD + 0x15)
#define MDD_SOFT_IRQ		(MSR_SB_MDD + 0x16)
#define MDD_SOFT_RESET		(MSR_SB_MDD + 0x17)
#define MDD_NORF_CNTRL		(MSR_SB_MDD + 0x18)
#define MDD_NORF_T01		(MSR_SB_MDD + 0x19)
#define MDD_NORF_T23		(MSR_SB_MDD + 0x1A)
#define MDD_NANDF_DATA		(MSR_SB_MDD + 0x1B)
#define MDD_NADF_CNTL		(MSR_SB_MDD + 0x1C)
#define MDD_AC_DMA		(MSR_SB_MDD + 0x1E)
#define MDD_KEL_CNTRL		(MSR_SB_MDD + 0x1F)

#define MDD_IRQM_YLOW		(MSR_SB_MDD + 0x20)
#define MDD_IRQM_YHIGH		(MSR_SB_MDD + 0x21)
#define MDD_IRQM_ZLOW		(MSR_SB_MDD + 0x22)
#define MDD_IRQM_ZHIGH		(MSR_SB_MDD + 0x23)
#define MDD_IRQM_PRIM		(MSR_SB_MDD + 0x24)
#define MDD_IRQM_LPC		(MSR_SB_MDD + 0x25)
#define MDD_IRQM_LXIRR		(MSR_SB_MDD + 0x26)
#define MDD_IRQM_HXIRR		(MSR_SB_MDD + 0x27)

#define MDD_MFGPT_IRQ		(MSR_SB_MDD + 0x28)
#define MDD_MFGPT_NR		(MSR_SB_MDD + 0x29)
#define MDD_MFGPT_RES0		(MSR_SB_MDD + 0x2A)
#define MDD_MFGPT_RES1		(MSR_SB_MDD + 0x2B)

#define MDD_FLOP_S3F2		(MSR_SB_MDD + 0x30)
#define MDD_FLOP_S3F7		(MSR_SB_MDD + 0x31)
#define MDD_FLOP_S372		(MSR_SB_MDD + 0x32)
#define MDD_FLOP_S377		(MSR_SB_MDD + 0x33)

#define MDD_PIC_S		(MSR_SB_MDD + 0x34)
#define MDD_PIT_S		(MSR_SB_MDD + 0x36)
#define MDD_PIT_CNTRL		(MSR_SB_MDD + 0x37)

#define MDD_UART1_MOD		(MSR_SB_MDD + 0x38)
#define MDD_UART1_DON		(MSR_SB_MDD + 0x39)
#define MDD_UART1_CONF		(MSR_SB_MDD + 0x3A)
#define MDD_UART2_MOD		(MSR_SB_MDD + 0x3C)
#define MDD_UART2_DON		(MSR_SB_MDD + 0x3D)
#define MDD_UART2_CONF		(MSR_SB_MDD + 0x3E)

#define MDD_DMA_MAP		(MSR_SB_MDD + 0x40)
#define MDD_DMA_SHAD1		(MSR_SB_MDD + 0x41)
#define MDD_DMA_SHAD2		(MSR_SB_MDD + 0x42)
#define MDD_DMA_SHAD3		(MSR_SB_MDD + 0x43)
#define MDD_DMA_SHAD4		(MSR_SB_MDD + 0x44)
#define MDD_DMA_SHAD5		(MSR_SB_MDD + 0x45)
#define MDD_DMA_SHAD6		(MSR_SB_MDD + 0x46)
#define MDD_DMA_SHAD7		(MSR_SB_MDD + 0x47)
#define MDD_DMA_SHAD8		(MSR_SB_MDD + 0x48)
#define MDD_DMA_SHAD9		(MSR_SB_MDD + 0x49)

#define MDD_LPC_EADDR		(MSR_SB_MDD + 0x4C)
#define MDD_LPC_ESTAT		(MSR_SB_MDD + 0x4D)
#define MDD_LPC_SIRQ		(MSR_SB_MDD + 0x4E)
#define MDD_LPC_RES		(MSR_SB_MDD + 0x4F)

#define MDD_PML_TMR		(MSR_SB_MDD + 0x50)
#define MDD_RTC_RAM_LO_CK	(MSR_SB_MDD + 0x54)
#define MDD_RTC_DOMA_IND	(MSR_SB_MDD + 0x55)
#define MDD_RTC_MONA_IND	(MSR_SB_MDD + 0x56)
#define MDD_RTC_CENTURY_OFFSET	(MSR_SB_MDD + 0x57)

/* LBUS Device Equates - */

/* SMBus */
#define	SMB_SDA		0x00
#define	SMB_STS		0x01
#define		SMB_STS_SLVSTP		(0x01 << 7)
#define		SMB_STS_SDAST		(0x01 << 6)
#define		SMB_STS_BER		(0x01 << 5)
#define		SMB_STS_NEGACK		(0x01 << 4)
#define		SMB_STS_STASTR		(0x01 << 3)
#define		SMB_STS_NMATCH		(0x01 << 2)
#define		SMB_STS_MASTER		(0x01 << 1)
#define		SMB_STS_XMIT		(0x01 << 0)

#define	SMB_CTRL_STS	0x02
#define		SMB_CSTS_TGSCL		(0x01 << 5)
#define		SMB_CSTS_TSDA		(0x01 << 4)
#define		SMB_CSTS_GCMTCH		(0x01 << 3)
#define		SMB_CSTS_MATCH		(0x01 << 2)
#define		SMB_CSTS_BB		(0x01 << 1)
#define		SMB_CSTS_BUSY		(0x01 << 0)

#define	SMB_CTRL1	0x03
#define		SMB_CTRL1_STASTRE	(0x01 << 7)
#define		SMB_CTRL1_NMINTE	(0x01 << 6)
#define		SMB_CTRL1_GCMEN		(0x01 << 5)
#define		SMB_CTRL1_ACK		(0x01 << 4)
#define		SMB_CTRL1_RSVD		(0x01 << 3)
#define		SMB_CTRL1_INTEN		(0x01 << 2)
#define		SMB_CTRL1_STOP		(0x01 << 1)
#define		SMB_CTRL1_START		(0x01 << 0)

#define	SMB_ADD		0x04
#define		SMB_ADD_SAEN		(0x01 << 7)

#define	SMB_CTRL2	0x05
#define		SMB_CTRL2_ENABLE	(0x01 << 0)

#define	SMB_CTRL3	0x06

/* GPIO */
#define GPIOL_0_SET	(1 << 0)
#define GPIOL_1_SET	(1 << 1)
#define GPIOL_2_SET	(1 << 2)
#define GPIOL_3_SET	(1 << 3)
#define GPIOL_4_SET	(1 << 4)
#define GPIOL_5_SET	(1 << 5)
#define GPIOL_6_SET	(1 << 6)
#define GPIOL_7_SET	(1 << 7)
#define GPIOL_8_SET	(1 << 8)
#define GPIOL_9_SET	(1 << 9)
#define GPIOL_10_SET	(1 << 10)
#define GPIOL_11_SET	(1 << 11)
#define GPIOL_12_SET	(1 << 12)
#define GPIOL_13_SET	(1 << 13)
#define GPIOL_14_SET	(1 << 14)
#define GPIOL_15_SET	(1 << 15)

#define GPIOL_0_CLEAR	(1 << 16)
#define GPIOL_1_CLEAR	(1 << 17)
#define GPIOL_2_CLEAR	(1 << 18)
#define GPIOL_3_CLEAR	(1 << 19)
#define GPIOL_4_CLEAR	(1 << 20)
#define GPIOL_5_CLEAR	(1 << 21)
#define GPIOL_6_CLEAR	(1 << 22)
#define GPIOL_7_CLEAR	(1 << 23)
#define GPIOL_8_CLEAR	(1 << 24)
#define GPIOL_9_CLEAR	(1 << 25)
#define GPIOL_10_CLEAR	(1 << 26)
#define GPIOL_11_CLEAR	(1 << 27)
#define GPIOL_12_CLEAR	(1 << 28)
#define GPIOL_13_CLEAR	(1 << 29)
#define GPIOL_14_CLEAR	(1 << 30)
#define GPIOL_15_CLEAR	(1 << 31)

#define GPIOH_16_SET	(1 << 0)
#define GPIOH_17_SET	(1 << 1)
#define GPIOH_18_SET	(1 << 2)
#define GPIOH_19_SET	(1 << 3)
#define GPIOH_20_SET	(1 << 4)
#define GPIOH_21_SET	(1 << 5)
#define GPIOH_22_SET	(1 << 6)
#define GPIOH_23_SET	(1 << 7)
#define GPIOH_24_SET	(1 << 8)
#define GPIOH_25_SET	(1 << 9)
#define GPIOH_26_SET	(1 << 10)
#define GPIOH_27_SET	(1 << 11)
#define GPIOH_28_SET	(1 << 12)
#define GPIOH_29_SET	(1 << 13)
#define GPIOH_30_SET	(1 << 14)
#define GPIOH_31_SET	(1 << 15)

#define GPIOH_16_CLEAR	(1 << 16)
#define GPIOH_17_CLEAR	(1 << 17)
#define GPIOH_18_CLEAR	(1 << 18)
#define GPIOH_19_CLEAR	(1 << 19)
#define GPIOH_20_CLEAR	(1 << 20)
#define GPIOH_21_CLEAR	(1 << 21)
#define GPIOH_22_CLEAR	(1 << 22)
#define GPIOH_23_CLEAR	(1 << 23)
#define GPIOH_24_CLEAR	(1 << 24)
#define GPIOH_25_CLEAR	(1 << 25)
#define GPIOH_26_CLEAR	(1 << 26)
#define GPIOH_27_CLEAR	(1 << 27)
#define GPIOH_28_CLEAR	(1 << 28)
#define GPIOH_29_CLEAR	(1 << 29)
#define GPIOH_30_CLEAR	(1 << 30)
#define GPIOH_31_CLEAR	(1 << 31)

/* GPIO LOW Bank Bit Registers */
#define GPIOL_OUTPUT_VALUE		(0x00)
#define GPIOL_OUTPUT_ENABLE		(0x04)
#define GPIOL_OUT_OPENDRAIN		(0x08)
#define GPIOL_OUTPUT_INVERT_ENABLE	(0x0C)
#define GPIOL_OUT_AUX1_SELECT		(0x10)
#define GPIOL_OUT_AUX2_SELECT		(0x14)
#define GPIOL_PULLUP_ENABLE		(0x18)
#define GPIOL_PULLDOWN_ENABLE		(0x1C)
#define GPIOL_INPUT_ENABLE		(0x20)
#define GPIOL_INPUT_INVERT_ENABLE	(0x24)
#define GPIOL_IN_FILTER_ENABLE		(0x28)
#define GPIOL_IN_EVENTCOUNT_ENABLE	(0x2C)
#define GPIOL_READ_BACK			(0x30)
#define GPIOL_IN_AUX1_SELECT		(0x34)
#define GPIOL_EVENTS_ENABLE		(0x38)
#define GPIOL_LOCK_ENABLE		(0x3C)
#define GPIOL_IN_POSEDGE_ENABLE		(0x40)
#define GPIOL_IN_NEGEDGE_ENABLE		(0x44)
#define GPIOL_IN_POSEDGE_STATUS		(0x48)
#define GPIOL_IN_NEGEDGE_STATUS		(0x4C)

/* GPIO  High Bank Bit Registers */
#define GPIOH_OUTPUT_VALUE		(0x80)
#define GPIOH_OUTPUT_ENABLE		(0x84)
#define GPIOH_OUT_OPENDRAIN		(0x88)
#define GPIOH_OUTPUT_INVERT_ENABLE	(0x8C)
#define GPIOH_OUT_AUX1_SELECT		(0x90)
#define GPIOH_OUT_AUX2_SELECT		(0x94)
#define GPIOH_PULLUP_ENABLE		(0x98)
#define GPIOH_PULLDOWN_ENABLE		(0x9C)
#define GPIOH_INPUT_ENABLE		(0xA0)
#define GPIOH_INPUT_INVERT_ENABLE	(0xA4)
#define GPIOH_IN_FILTER_ENABLE		(0xA8)
#define GPIOH_IN_EVENTCOUNT_ENABLE	(0xAC)
#define GPIOH_READ_BACK			(0xB0)
#define GPIOH_IN_AUX1_SELECT		(0xB4)
#define GPIOH_EVENTS_ENABLE		(0xB8)
#define GPIOH_LOCK_ENABLE		(0xBC)
#define GPIOH_IN_POSEDGE_ENABLE		(0xC0)
#define GPIOH_IN_NEGEDGE_ENABLE		(0xC4)
#define GPIOH_IN_POSEDGE_STATUS		(0xC8)
#define GPIOH_IN_NEGEDGE_STATUS		(0xCC)

/* Input Conditioning Function Registers */
#define GPIO_00_FILTER_AMOUNT		(0x50)
#define GPIO_00_FILTER_COUNT		(0x52)
#define GPIO_00_EVENT_COUNT		(0x54)
#define GPIO_00_EVENTCOMPARE_VALUE	(0x56)
#define GPIO_01_FILTER_AMOUNT		(0x58)
#define GPIO_01_FILTER_COUNT		(0x5A)
#define GPIO_01_EVENT_COUNT		(0x5C)
#define GPIO_01_EVENTCOMPARE_VALUE	(0x5E)
#define GPIO_02_FILTER_AMOUNT		(0x60)
#define GPIO_02_FILTER_COUNT		(0x62)
#define GPIO_02_EVENT_COUNT		(0x64)
#define GPIO_02_EVENTCOMPARE_VALUE	(0x66)
#define GPIO_03_FILTER_AMOUNT		(0x68)
#define GPIO_03_FILTER_COUNT		(0x6A)
#define GPIO_03_EVENT_COUNT		(0x6C)
#define GPIO_03_EVENTCOMPARE_VALUE	(0x6E)
#define GPIO_04_FILTER_AMOUNT		(0x70)
#define GPIO_04_FILTER_COUNT		(0x72)
#define GPIO_04_EVENT_COUNT		(0x74)
#define GPIO_04_EVENTCOMPARE_VALUE	(0x76)
#define GPIO_05_FILTER_AMOUNT		(0x78)
#define GPIO_05_FILTER_COUNT		(0x7A)
#define GPIO_05_EVENT_COUNT		(0x7C)
#define GPIO_05_EVENTCOMPARE_VALUE	(0x7E)
#define GPIO_06_FILTER_AMOUNT		(0xD0)
#define GPIO_06_FILTER_COUNT		(0xD2)
#define GPIO_06_EVENT_COUNT		(0xD4)
#define GPIO_06_EVENTCOMPARE_VALUE	(0xD6)
#define GPIO_07_FILTER_AMOUNT		(0xD8)
#define GPIO_07_FILTER_COUNT		(0xDA)
#define GPIO_07_EVENT_COUNT		(0xDC)
#define GPIO_07_EVENTCOMPARE_VALUE	(0xDE)

/* R/W GPIO Interrupt &PME Mapper Registers */
#define GPIO_MAPPER_X		(0xE0)
#define GPIO_MAPPER_Y		(0xE4)
#define GPIO_MAPPER_Z		(0xE8)
#define GPIO_MAPPER_W		(0xEC)
#define GPIO_FE_SELECT_0	(0xF0)
#define GPIO_FE_SELECT_1	(0xF1)
#define GPIO_FE_SELECT_2	(0xF2)
#define GPIO_FE_SELECT_3	(0xF3)
#define GPIO_FE_SELECT_4	(0xF4)
#define GPIO_FE_SELECT_5	(0xF5)
#define GPIO_FE_SELECT_6	(0xF6)
#define GPIO_FE_SELECT_7	(0xF7)

/* Event Counter Decrement Registers */
#define GPIOL_IN_EVENT_DECREMENT	(0xF8)
#define GPIOH_IN_EVENT_DECREMENT	(0xFC)

/* PMC register */
#define PM_SSD		(0x00)
#define PM_SCXA		(0x04)
#define PM_SCYA		(0x08)
#define PM_SODA		(0x0C)
#define PM_SCLK		(0x10)
#define PM_SED		(0x14)
#define PM_SCXD		(0x18)
#define PM_SCYD		(0x1C)
#define PM_SIDD		(0x20)
#define PM_WKD		(0x30)
#define PM_WKXD		(0x34)
#define PM_RD		(0x38)
#define PM_WKXA		(0x3C)
#define PM_FSD		(0x40)
#define PM_TSD		(0x44)
#define PM_PSD		(0x48)
#define PM_NWKD		(0x4C)
#define PM_AWKD		(0x50)
#define PM_SSC		(0x54)

/* FLASH device macros */
#define FLASH_TYPE_NONE		0	/* No flash device installed */
#define FLASH_TYPE_NAND		1	/* NAND device */
#define FLASH_TYPE_NOR		2	/* NOR device */

#define FLASH_IF_MEM		1	/* Memory or memory-mapped I/O interface for Flash device */
#define FLASH_IF_IO		2	/* I/O interface for Flash device */

/* Flash Memory Mask values */
#define FLASH_MEM_DEFAULT	0x00000000
#define FLASH_MEM_4K		0xFFFFF000
#define FLASH_MEM_8K		0xFFFFE000
#define FLASH_MEM_16K		0xFFFFC000
#define FLASH_MEM_128K		0xFFFE0000
#define FLASH_MEM_512K		0xFFFC0000
#define FLASH_MEM_4M		0xFFC00000
#define FLASH_MEM_8M		0xFF800000
#define FLASH_MEM_16M		0xFF000000

/* Flash IO Mask values */
#define FLASH_IO_DEFAULT	0x00000000
#define FLASH_IO_16B		0x0000FFF0
#define FLASH_IO_32B		0x0000FFE0
#define FLASH_IO_64B		0x0000FFC0
#define FLASH_IO_128B		0x0000FF80
#define FLASH_IO_256B		0x0000FF00

#if !defined(__ASSEMBLER__)
#if defined(__PRE_RAM__)
void cs5536_setup_onchipuart(int uart);
void cs5536_disable_internal_uart(void);
void cs5536_early_setup(void);

void cs5536_enable_smbus(void);
int smbus_read_byte(unsigned device, unsigned address);
#else
void chipsetinit(void);
#endif
#endif

#endif /* _CS5536_H */
