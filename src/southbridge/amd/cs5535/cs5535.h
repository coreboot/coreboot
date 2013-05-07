/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Nils Jacobs
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _CS5535_H
#define _CS5535_H

/*	SouthBridge Equates */
#define CS5535_GLINK_PORT_NUM	0x02	/* port of the SouthBridge */
#define NB_PCI			((2	<< 29) + (4 << 26))	/* NB GLPCI is in the same location on all Geodes. */
#define MSR_SB			((CS5535_GLINK_PORT_NUM << 23) + NB_PCI)	/* address to the SouthBridge */
#define SB_SHIFT		20	/* 29 -> 26 -> 23 -> 20...... When making a SB address uses this shift. */

#define CS5535_DEV_NUM 		0x0F	/* default PCI device number for CS5535 */
#define SMBUS_IO_BASE		0x6000
#define GPIO_IO_BASE		0x6100
#define MFGPT_IO_BASE		0x6200
#define ACPI_IO_BASE		0x9C00
#define PMS_IO_BASE		0x9D00

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
#define GLIU_SB_GLD_MSR_PM	(MSR_SB_GLIU + 0x04)

/* USB1 */
#define USB1_SB_GLD_MSR_CONF	(MSR_SB_USB1 + 0x01)
#define USB1_SB_GLD_MSR_PM	(MSR_SB_USB1 + 0x04)

/* USB2 */
#define USB2_SB_GLD_MSR_CONF	(MSR_SB_USB2 + 0x01)
#define USB2_SB_GLD_MSR_PM	(MSR_SB_USB2 + 0x04)

/* ATA */
#define ATA_SB_GLD_MSR_CONF	(MSR_SB_ATA + 0x01)
#define ATA_SB_GLD_MSR_ERR	(MSR_SB_ATA + 0x03)
#define ATA_SB_GLD_MSR_PM	(MSR_SB_ATA + 0x04)
#define ATA_SB_IDE_CFG		(MSR_SB_ATA + 0x10)

/* AC97 */
#define AC97_SB_GLD_MSR_CONF	(MSR_SB_AC97 + 0x01)
#define AC97_SB_GLD_MSR_PM	(MSR_SB_AC97 + 0x04)

/* GLPCI */
#define GLPCI_SB_GLD_MSR_PM	(MSR_SB_GLPCI + 0x04)
#define GLPCI_SB_CTRL		(MSR_SB_GLPCI + 0x10)
#define GLPCI_CRTL_PPIDE_SET	(1 << 17)

/* GLCP */
#define GLCP_SB_GLD_MSR_PM	(MSR_SB_GLCP + 0x04)

/* MDD */
#define MDD_SB_GLD_MSR_CONF	(MSR_SB_MDD + 0x01)
#define MDD_SB_GLD_MSR_PM	(MSR_SB_MDD + 0x04)
#define MDD_LBAR_SMB		(MSR_SB_MDD + 0x0B)
#define MDD_LBAR_GPIO		(MSR_SB_MDD + 0x0C)
#define MDD_LBAR_MFGPT		(MSR_SB_MDD + 0x0D)
#define MDD_LBAR_ACPI		(MSR_SB_MDD + 0x0E)
#define MDD_LBAR_PMS		(MSR_SB_MDD + 0x0F)
#define MDD_LBAR_FLSH0		(MSR_SB_MDD + 0x10)
#define MDD_LBAR_FLSH1		(MSR_SB_MDD + 0x11)
#define MDD_LBAR_FLSH2		(MSR_SB_MDD + 0x12)
#define MDD_LBAR_FLSH3		(MSR_SB_MDD + 0x13)
#define MDD_PIN_OPT		(MSR_SB_MDD + 0x15)
#define MDD_NORF_CNTRL		(MSR_SB_MDD + 0x18)

/* GPIO */
#define GPIOL_2_SET		(1 << 2)

/* GPIO LOW Bank Bit Registers */
#define GPIOL_INPUT_ENABLE	(0x20)
#define GPIOL_IN_AUX1_SELECT	(0x34)

/* FLASH device macros */
#define FLASH_TYPE_NONE		0	/* No flash device installed */
#define FLASH_TYPE_NAND 	1	/* NAND device */

#define FLASH_IF_MEM		1	/* Memory or memory-mapped I/O interface for Flash device */

/* Flash Memory Mask values */
#define FLASH_MEM_4K		0xFFFFF000

#if !defined(__ASSEMBLER__)
#if defined(__PRE_RAM__)
void cs5535_disable_internal_uart(void);
#else
void chipsetinit(void);
#endif
#endif

#endif				/* _CS5535_H */
