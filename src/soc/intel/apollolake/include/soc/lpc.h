/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
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

#ifndef _SOC_APOLLOLAKE_LPC_H
#define _SOC_APOLLOLAKE_LPC_H

/* PCI Configuration Space (D31:F0): LPC */
#define SERIRQ_CNTL		0x64 /* Serial IRQ Control Register */
#define LPC_IO_DEC		0x80 /* IO Decode Ranges Register */
#define LPC_EN			0x82 /* LPC IF Enables Register */
#define  LPC_EN_COMA		(1 << 0) /* COM port A */
#define  LPC_EN_COMB		(1 << 1) /* COM port B */
#define  LPC_EN_PARP		(1 << 2) /* Parallel port */
#define  LPC_EN_FLP		(1 << 3) /* Floppy */
#define  LPC_EN_LGAME		(1 << 8) /* Low Gameport, 0x200-0x207 */
#define  LPC_EN_HGAME		(1 << 9) /* High Gameport, 0x208-0x20f */
#define  LPC_EN_KB		(1 << 10) /* Keyboard, 0x60, 0x64 */
#define  LPC_EN_MC1		(1 << 11) /* Microcontroller #1, 0x62, 0x66 */
#define  LPC_EN_MC2		(1 << 13) /* Microcontroller #2, 0x4e, 0x4f */
#define  LPC_EN_SIO		(1 << 12) /* Super IO, 0x2e, 0x2f */

#define LPC_GEN1_DEC		0x84 /* LPC IF Generic Decode Range 1 */
#define LPC_GEN2_DEC		0x88 /* LPC IF Generic Decode Range 2 */
#define LPC_GEN3_DEC		0x8C /* LPC IF Generic Decode Range 3 */
#define LPC_GEN4_DEC		0x90 /* LPC IF Generic Decode Range 4 */

#endif //_SOC_APOLLOLAKE_LPC_H
