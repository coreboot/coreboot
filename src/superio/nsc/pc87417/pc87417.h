/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SUPERIO_NSC_PC87417_PC87417_H
#define SUPERIO_NSC_PC87417_PC87417_H

#define PC87417_FDC  0x00 /* Floppy */
#define PC87417_PP   0x01 /* Parallel Port */
#define PC87417_SP2  0x02 /* Com2 */
#define PC87417_SP1  0x03 /* Com1 */
#define PC87417_SWC  0x04 /* System Wake-Up Controller (SWC) */
#define PC87417_KBCM 0x05 /* Mouse */
#define PC87417_KBCK 0x06 /* Keyboard */
#define PC87417_GPIO 0x07
#define PC87417_XBUS 0x0F
#define PC87417_RTC  0x10

#define PC87417_GPIO_DEV PNP_DEV(0x2e, PC87417_GPIO)
/* This is to get around a romcc bug */
/* #define PC87417_XBUS_DEV PNP_DEV(0x2e, PC87417_XBUS) */
#define PC87417_XBUS_DEV PNP_DEV(0x2e, 0x0f)

#define PC87417_GPSEL		0xf0
#define PC87417_GPCFG1		0xf1
#define PC87417_GPEVR		0xf2
#define PC87417_GPCFG2		0xf3
#define PC87417_EXTCFG		0xf4
#define PC87417_IOEXT1A		0xf5
#define PC87417_IOEXT1B		0xf6
#define PC87417_IOEXT2A		0xf7
#define PC87417_IOEXT2B		0xf8

#define PC87417_GPDO_0		0x00
#define PC87417_GPDI_0		0x01
#define PC87417_GPDO_1		0x02
#define PC87417_GPDI_1		0x03
#define PC87417_GPEVEN_1	0x04
#define PC87417_GPEVST_1	0x05
#define PC87417_GPDO_2		0x06
#define PC87417_GPDI_2		0x07
#define PC87417_GPDO_3		0x08
#define PC87417_GPDI_3		0x09
#define PC87417_GPDO_4		0x0a
#define PC87417_GPDI_4		0x0b
#define PC87417_GPEVEN_4	0x0c
#define PC87417_GPEVST_4	0x0d
#define PC87417_GPDO_5		0x0e
#define PC87417_GPDI_5		0x0f
#define PC87417_GPDO_6		0x10
#define PC87417_GPDO_7A		0x11
#define PC87417_GPDO_7B		0x12
#define PC87417_GPDO_7C		0x13
#define PC87417_GPDO_7D		0x14
#define PC87417_GPDI_7A		0x15
#define PC87417_GPDI_7B		0x16
#define PC87417_GPDI_7C		0x17
#define PC87417_GPDI_7D		0x18

#define PC87417_XIOCNF		0xf0
#define PC87417_XIOBA1H		0xf1
#define PC87417_XIOBA1L		0xf2
#define PC87417_XIOSIZE1	0xf3
#define PC87417_XIOBA2H		0xf4
#define PC87417_XIOBA2L		0xf5
#define PC87417_XIOSIZE2	0xf6
#define PC87417_XMEMCNF1	0xf7
#define PC87417_XMEMCNF2	0xf8
#define PC87417_XMEMBAH		0xf9
#define PC87417_XMEMBAL		0xfa
#define PC87417_XMEMSIZE	0xfb
#define PC87417_XIRQMAP1	0xfc
#define PC87417_XIRQMAP2	0xfd
#define PC87417_XBIMM		0xfe
#define PC87417_XBBSL		0xff

#define PC87417_XBCNF		0x00
#define PC87417_XZCNF0		0x01
#define PC87417_XZCNF1		0x02
#define PC87417_XIRQC0		0x04
#define PC87417_XIRQC1		0x05
#define PC87417_XIRQC2		0x06
#define PC87417_XIMA0		0x08
#define PC87417_XIMA1		0x09
#define PC87417_XIMA2		0x0a
#define PC87417_XIMA3		0x0b
#define PC87417_XIMD		0x0c
#define PC87417_XZCNF2		0x0d
#define PC87417_XZCNF3		0x0e
#define PC87417_XZM0		0x0f
#define PC87417_XZM1		0x10
#define PC87417_XZM2		0x11
#define PC87417_XZM3		0x12
#define PC87417_HAP0		0x13
#define PC87417_HAP1		0x14
#define PC87417_XSCNF		0x15
#define PC87417_XWBCNF		0x16

#if defined(__PRE_RAM__)
void pc87417_enable_serial(device_t dev, u16 iobase);
void pc87417_enable_dev(device_t dev);
#endif

#endif
