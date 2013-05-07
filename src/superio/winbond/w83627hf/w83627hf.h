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

#ifndef SUPERIO_WINBOND_W83627HF_W83627HF_H
#define SUPERIO_WINBOND_W83627HF_W83627HF_H

#define W83627HF_FDC              0   /* Floppy */
#define W83627HF_PP               1   /* Parallel port */
#define W83627HF_SP1              2   /* Com1 */
#define W83627HF_SP2              3   /* Com2 */
#define W83627HF_KBC              5   /* PS/2 keyboard & mouse */
#define W83627HF_CIR              6
#define W83627HF_GAME_MIDI_GPIO1  7
#define W83627HF_GPIO2            8
#define W83627HF_GPIO3            9
#define W83627HF_ACPI            10
#define W83627HF_HWM             11   /* Hardware monitor */

/* #define W83627HF_GPIO_DEV PNP_DEV(0x2e, W83627HF_GPIO) */
/* #define W83627HF_XBUS_DEV PNP_DEV(0x2e, W83627HF_XBUS) */

#define W83627HF_GPSEL		0xf0
#define W83627HF_GPCFG1		0xf1
#define W83627HF_GPEVR		0xf2
#define W83627HF_GPCFG2		0xf3
#define W83627HF_EXTCFG		0xf4
#define W83627HF_IOEXT1A	0xf5
#define W83627HF_IOEXT1B	0xf6
#define W83627HF_IOEXT2A	0xf7
#define W83627HF_IOEXT2B	0xf8

#define W83627HF_GPDO_0		0x00
#define W83627HF_GPDI_0		0x01
#define W83627HF_GPDO_1		0x02
#define W83627HF_GPDI_1		0x03
#define W83627HF_GPEVEN_1	0x04
#define W83627HF_GPEVST_1	0x05
#define W83627HF_GPDO_2		0x06
#define W83627HF_GPDI_2		0x07
#define W83627HF_GPDO_3		0x08
#define W83627HF_GPDI_3		0x09
#define W83627HF_GPDO_4		0x0a
#define W83627HF_GPDI_4		0x0b
#define W83627HF_GPEVEN_4	0x0c
#define W83627HF_GPEVST_4	0x0d
#define W83627HF_GPDO_5		0x0e
#define W83627HF_GPDI_5		0x0f
#define W83627HF_GPDO_6		0x10
#define W83627HF_GPDO_7A	0x11
#define W83627HF_GPDO_7B	0x12
#define W83627HF_GPDO_7C	0x13
#define W83627HF_GPDO_7D	0x14
#define W83627HF_GPDI_7A	0x15
#define W83627HF_GPDI_7B	0x16
#define W83627HF_GPDI_7C	0x17
#define W83627HF_GPDI_7D	0x18

#define W83627HF_XIOCNF		0xf0
#define W83627HF_XIOBA1H	0xf1
#define W83627HF_XIOBA1L	0xf2
#define W83627HF_XIOSIZE1	0xf3
#define W83627HF_XIOBA2H	0xf4
#define W83627HF_XIOBA2L	0xf5
#define W83627HF_XIOSIZE2	0xf6
#define W83627HF_XMEMCNF1	0xf7
#define W83627HF_XMEMCNF2	0xf8
#define W83627HF_XMEMBAH	0xf9
#define W83627HF_XMEMBAL	0xfa
#define W83627HF_XMEMSIZE	0xfb
#define W83627HF_XIRQMAP1	0xfc
#define W83627HF_XIRQMAP2	0xfd
#define W83627HF_XBIMM		0xfe
#define W83627HF_XBBSL		0xff

#define W83627HF_XBCNF		0x00
#define W83627HF_XZCNF0		0x01
#define W83627HF_XZCNF1		0x02
#define W83627HF_XIRQC0		0x04
#define W83627HF_XIRQC1		0x05
#define W83627HF_XIRQC2		0x06
#define W83627HF_XIMA0		0x08
#define W83627HF_XIMA1		0x09
#define W83627HF_XIMA2		0x0a
#define W83627HF_XIMA3		0x0b
#define W83627HF_XIMD		0x0c
#define W83627HF_XZCNF2		0x0d
#define W83627HF_XZCNF3		0x0e
#define W83627HF_XZM0		0x0f
#define W83627HF_XZM1		0x10
#define W83627HF_XZM2		0x11
#define W83627HF_XZM3		0x12
#define W83627HF_HAP0		0x13
#define W83627HF_HAP1		0x14
#define W83627HF_XSCNF		0x15
#define W83627HF_XWBCNF		0x16

#if defined(__PRE_RAM__)
void w83627hf_disable_dev(device_t dev);
void w83627hf_enable_dev(device_t dev, u16 iobase);
void w83627hf_enable_serial(device_t dev, u16 iobase);
void w83627hf_set_clksel_48(device_t dev);
#endif

#endif
