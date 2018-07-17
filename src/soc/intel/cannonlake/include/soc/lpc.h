/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Intel Corporation.
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

#ifndef _SOC_CANNONLAKE_LPC_H_
#define _SOC_CANNONLAKE_LPC_H_

/* PCI Configuration Space (D31:F0): LPC */
#define SCI_IRQ_SEL		(7 << 0)
#define SCIS_IRQ9		0
#define SCIS_IRQ10		1
#define SCIS_IRQ11		2
#define SCIS_IRQ20		4
#define SCIS_IRQ21		5
#define SCIS_IRQ22		6
#define SCIS_IRQ23		7
#define SERIRQ_CNTL		0x64
#define LPC_IO_DEC		0x80 /* IO Decode Ranges Register */
#define   COMA_RANGE		0x0 /* 0x3F8 - 0x3FF COM1*/
#define   COMB_RANGE		0x1 /* 0x2F8 - 0x2FF COM2*/
#define LPC_EN			0x82 /* LPC IF Enables Register */
#define   MC2_LPC_EN		(1 << 13) /* 0x4e/0x4f */
#define   SE_LPC_EN		(1 << 12) /* 0x2e/0x2f */
#define   MC1_LPC_EN		(1 << 11) /* 0x62/0x66 */
#define   KBC_LPC_EN		(1 << 10) /* 0x60/0x64 */
#define   GAMEH_LPC_EN		(1 << 9)  /* 0x208/0x20f */
#define   GAMEL_LPC_EN		(1 << 8)  /* 0x200/0x207 */
#define   FDD_LPC_EN		(1 << 3)  /* Floppy Drive Enable */
#define   LPT_LPC_EN		(1 << 2)  /* Parallel Port Enable */
#define   COMB_LPC_EN		(1 << 1)  /* Com Port B Enable */
#define   COMA_LPC_EN		(1 << 0)  /* Com Port A Enable */
#define LPC_GEN1_DEC		0x84 /* LPC IF Generic Decode Range 1 */
#define LPC_GEN2_DEC		0x88 /* LPC IF Generic Decode Range 2 */
#define LPC_GEN3_DEC		0x8c /* LPC IF Generic Decode Range 3 */
#define LPC_GEN4_DEC		0x90 /* LPC IF Generic Decode Range 4 */
#define LGMR			0x98 /* LPC Generic Memory Range */
#define BIOS_CNTL		0xdc
#define   LPC_BC_BILD		(1 << 7) /* BILD */
#define   LPC_BC_LE		(1 << 1) /* LE */
#define   LPC_BC_EISS		(1 << 5) /* EISS */
#define PCCTL			0xE0 /* PCI Clock Control */
#define   CLKRUN_EN		(1 << 0)

/*
 * This function will help to differentiate between 2 PCH on single type of soc.
 * Since same soc may have LP series pch or H series PCH, we need to
 * differentiate by reading upper 8 bits of PCH device ids.
 *
 * Return:
 *	Return PCH_LP or PCH_H macro in case of respective device ID found.
 *	PCH_UNKNOWN_SERIES in case of invalid device ID.
 */
uint8_t get_pch_series(void);

#endif
