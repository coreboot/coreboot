/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#ifndef _BROADWELL_SATA_H_
#define _BROADWELL_SATA_H_

#define SATA_SIRI		0xa0 /* SATA Indexed Register Index */
#define SATA_SIRD		0xa4 /* SATA Indexed Register Data */
#define SATA_SP			0xd0 /* Scratchpad */

/* SATA IOBP Registers */
#define SATA_IOBP_SP0_SECRT88	0xea002688
#define SATA_IOBP_SP1_SECRT88	0xea002488

#define SATA_SECRT88_VADJ_MASK	0xff
#define SATA_SECRT88_VADJ_SHIFT	16

#define SATA_IOBP_SP0DTLE_DATA	0xea002750
#define SATA_IOBP_SP0DTLE_EDGE	0xea002754
#define SATA_IOBP_SP1DTLE_DATA	0xea002550
#define SATA_IOBP_SP1DTLE_EDGE	0xea002554

#define SATA_DTLE_MASK		0xF
#define SATA_DTLE_DATA_SHIFT	24
#define SATA_DTLE_EDGE_SHIFT	16

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

#endif
