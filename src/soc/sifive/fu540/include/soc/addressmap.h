/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Jonathan Neuschäfer
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

#define FU540_MSEL		0x00001000
#define FU540_DTIM		0x01000000
#define FU540_L2LIM		0x08000000
#define FU540_UART0		0x10010000
#define FU540_UART(x)		(FU540_UART0 + 0x1000 * (x))
#define FU540_PRCI		0x10000000
#define FU540_QSPI0		0x10040000
#define FU540_QSPI1		0x10041000
#define FU540_QSPI2		0x10050000
#define FU540_GPIO		0x10060000
#define FU540_OTP		0x10070000
#define FU540_PINCTRL		0x10080000
#define FU540_ETHMAC		0x10090000
#define FU540_ETHMGMT		0x100a0000
#define FU540_DDRCTRL		0x100b0000
#define FU540_DDRMGMT		0x100c0000
#define FU540_QSPI0FLASH	0x20000000
#define FU540_QSPI1FLASH	0x30000000
#define FU540_DRAM		0x80000000
