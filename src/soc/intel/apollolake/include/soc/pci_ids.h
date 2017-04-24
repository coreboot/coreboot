/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
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

#ifndef _SOC_APOLLOLAKE_PCI_IDS_H_
#define _SOC_APOLLOLAKE_PCI_IDS_H_

#define PCI_DEVICE_ID_APOLLOLAKE_NB		0x5af0		/* 00:00.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_IGD_HD_505	0x5a84		/* 00:02.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_IGD_HD_500	0x5a85		/* 00:02.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_P2SB		0x5a92		/* 00:0d.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_PMC		0x5a94		/* 00:0d.1 */
#define PCI_DEVICE_ID_APOLLOLAKE_HWSEQ_SPI	0x5a96		/* 00:0d.2 */
#define PCI_DEVICE_ID_APOLLOLAKE_SRAM		0x5aec		/* 00:0d.3 */
#define PCI_DEVICE_ID_APOLLOLAKE_AUDIO		0x5a98		/* 00:0e.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_SATA		0x5ae0		/* 00:12.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_I2C0		0x5aac		/* 00:16.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_I2C1		0x5aae		/* 00:16.1 */
#define PCI_DEVICE_ID_APOLLOLAKE_I2C2		0x5ab0		/* 00:16.2 */
#define PCI_DEVICE_ID_APOLLOLAKE_I2C3		0x5ab2		/* 00:16.3 */
#define PCI_DEVICE_ID_APOLLOLAKE_I2C4		0x5ab4		/* 00:17.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_I2C5		0x5ab6		/* 00:17.1 */
#define PCI_DEVICE_ID_APOLLOLAKE_I2C6		0x5ab8		/* 00:17.2 */
#define PCI_DEVICE_ID_APOLLOLAKE_I2C7		0x5aba		/* 00:17.3 */
#define PCI_DEVICE_ID_APOLLOLAKE_UART0		0x5abc		/* 00:18.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_UART1		0x5abe		/* 00:18.1 */
#define PCI_DEVICE_ID_APOLLOLAKE_UART2		0x5ac0		/* 00:18.2 */
#define PCI_DEVICE_ID_APOLLOLAKE_UART3		0x5aee		/* 00:18.3 */
#define PCI_DEVICE_ID_APOLLOLAKE_SPI0		0x5ac2		/* 00:19.0 */
#define PCI_DEVICE_ID_APOLLOLAKE_SPI1		0x5ac4		/* 00:19.1 */
#define PCI_DEVICE_ID_APOLLOLAKE_SPI2		0x5ac6		/* 00:19.2 */
#define PCI_DEVICE_ID_APOLLOLAKE_LPC		0x5ae8		/* 00:1f.0 */

#endif /* _SOC_APOLLOLAKE_PCI_IDS_H_ */
