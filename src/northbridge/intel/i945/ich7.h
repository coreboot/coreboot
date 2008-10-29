/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef __NORTHBRIDGE_INTEL_I945_ICH7_H__
#define __NORTHBRIDGE_INTEL_I945_ICH7_H__ 1

/* Southbridge IO BARs */
/* TODO Make sure these don't get changed by stage2 */
#define GPIOBASE		0x48
#define DEFAULT_GPIOBASE	0x480

#define PMBASE		0x40
#define DEFAULT_PMBASE	0x500

/* Root Complex Register Block */
#define RCBA		0xf0
#define DEFAULT_RCBA		0xfed1c000

#define RCBA8(x) *((volatile u8 *)(DEFAULT_RCBA + x))
#define RCBA16(x) *((volatile u16 *)(DEFAULT_RCBA + x))
#define RCBA32(x) *((volatile u32 *)(DEFAULT_RCBA + x))

#define VCH		0x0000	/* 32bit */
#define VCAP1		0x0004	/* 32bit */
#define VCAP2		0x0008	/* 32bit */
#define PVC		0x000c	/* 16bit */
#define PVS		0x000e	/* 16bit */

#define V0CAP		0x0010	/* 32bit */
#define V0CTL		0x0014	/* 32bit */
#define V0STS		0x001a	/* 16bit */

#define V1CAP		0x001c	/* 32bit */
#define V1CTL		0x0020	/* 32bit */
#define V1STS		0x0026	/* 16bit */

#define RCTCL		0x0100	/* 32bit */
#define ESD		0x0104	/* 32bit */
#define ULD		0x0110	/* 32bit */
#define ULBA		0x0118	/* 64bit */

#define RP1D		0x0120	/* 32bit */
#define RP1BA		0x0128	/* 64bit */
#define RP2D		0x0130	/* 32bit */
#define RP2BA		0x0138	/* 64bit */
#define RP3D		0x0140	/* 32bit */
#define RP3BA		0x0138	/* 64bit */
#define RP4D		0x0150	/* 32bit */
#define RP4BA		0x0158	/* 64bit */
#define HDD		0x0160	/* 32bit */
#define HDBA		0x0168	/* 64bit */
#define RP5D		0x0170	/* 32bit */
#define RP5BA		0x0178	/* 64bit */
#define RP6D		0x0180	/* 32bit */
#define RP6BA		0x0188	/* 64bit */

#define ILCL		0x01a0	/* 32bit */
#define LCAP		0x01a4	/* 32bit */
#define LCTL		0x01a8	/* 16bit */
#define LSTS		0x01aa	/* 16bit */

#define RPC		0x0224	/* 32bit */
#define RPFN		0x0238	/* 32bit */

#define TRSR		0x1e00	/*  8bit */
#define TRCR		0x1e10	/* 64bit */
#define TWDR		0x1e18	/* 64bit */

#define IOTR0		0x1e80	/* 64bit */
#define IOTR1		0x1e88	/* 64bit */
#define IOTR2		0x1e90	/* 64bit */
#define IOTR3		0x1e98	/* 64bit */

#define TCTL		0x3000	/*  8bit */

#define D31IP		0x3100	/* 32bit */
#define D30IP		0x3104	/* 32bit */
#define D29IP		0x3108	/* 32bit */
#define D28IP		0x310c	/* 32bit */
#define D27IP		0x3110	/* 32bit */
#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define OIC		0x31ff	/*  8bit */

#define RC		0x3400	/* 32bit */
#define HPTC		0x3404	/* 32bit */
#define GCS		0x3410	/* 32bit */
#define BUC		0x3414	/* 32bit */
#define FD		0x3418	/* 32bit */
#define CG		0x341c	/* 32bit */

/* Function Disable (FD) register values.
 * Setting a bit disables the corresponding
 * feature.
 * Not all features might be disabled on
 * all chipsets. Esp. ICH-7U is picky.
 */
#define FD_PCIE6	(1 << 21)
#define FD_PCIE5	(1 << 20)
#define FD_PCIE4	(1 << 19)
#define FD_PCIE3	(1 << 18)
#define FD_PCIE2	(1 << 17)
#define FD_PCIE1	(1 << 16)
#define FD_EHCI		(1 << 15)
#define FD_LPCB		(1 << 14)

/* UHCI must be disabled from 4 downwards.
 * If UHCI controllers get disabled, EHCI
 * must know about it, too! */
#define FD_UHCI4	(1 << 11)
#define FD_UHCI34	(1 << 10) | FD_UHCI4
#define FD_UHCI234	(1 <<  9) | FD_UHCI3
#define FD_UHCI1234	(1 <<  8) | FD_UHCI2

#define FD_INTLAN	(1 <<  7)
#define FD_ACMOD	(1 <<  6)
#define FD_ACAUD	(1 <<  5)
#define FD_HDAUD	(1 <<  4)
#define FD_SMBUS	(1 <<  3)
#define FD_SATA		(1 <<  2)
#define FD_PATA		(1 <<  1)

#endif
