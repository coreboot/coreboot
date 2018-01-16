/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors.  All rights reserved.
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

#ifndef SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H
#define SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H

/*
 * The DnnIR registers use common RCBA offsets across these chipsets:
 * bd82x6x, i82801, i89xx, ibexpeak, lynxpoint
 *
 * However not all registers are in use on all of these.
 */

#ifndef __ACPI__
#define DEFAULT_RCBA		((u8 *)0xfed1c000)
#else
#define DEFAULT_RCBA	        0xfed1c000
#endif

#ifndef __ACPI__

#define RCBA8(x) (*((volatile u8 *)(DEFAULT_RCBA + x)))
#define RCBA16(x) (*((volatile u16 *)(DEFAULT_RCBA + x)))
#define RCBA32(x) (*((volatile u32 *)(DEFAULT_RCBA + x)))

#define RCBA_AND_OR(bits, x, and, or) \
	(RCBA##bits(x) = ((RCBA##bits(x) & (and)) | (or)))
#define RCBA8_AND_OR(x, and, or)  RCBA_AND_OR(8, x, and, or)
#define RCBA16_AND_OR(x, and, or) RCBA_AND_OR(16, x, and, or)
#define RCBA32_AND_OR(x, and, or) RCBA_AND_OR(32, x, and, or)
#define RCBA32_OR(x, or) RCBA_AND_OR(32, x, ~0UL, or)


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
#define RP3BA		0x0148	/* 64bit */
#define RP4D		0x0150	/* 32bit */
#define RP4BA		0x0158	/* 64bit */
#define HDD		0x0160	/* 32bit */
#define HDBA		0x0168	/* 64bit */
#define RP5D		0x0170	/* 32bit */
#define RP5BA		0x0178	/* 64bit */
#define RP6D		0x0180	/* 32bit */
#define RP6BA		0x0188	/* 64bit */

#define RPC		0x0400	/* 32bit */
#define RPFN		0x0404	/* 32bit */

/* Root Port configuratinon space hide */
#define RPFN_HIDE(port)         (1 << (((port) * 4) + 3))
/* Get the function number assigned to a Root Port */
#define RPFN_FNGET(reg, port)    (((reg) >> ((port) * 4)) & 7)
/* Set the function number for a Root Port */
#define RPFN_FNSET(port, func)   (((func) & 7) << ((port) * 4))
/* Root Port function number mask */
#define RPFN_FNMASK(port)       (7 << ((port) * 4))

#define TRSR		0x1e00	/*  8bit */
#define TRCR		0x1e10	/* 64bit */
#define TWDR		0x1e18	/* 64bit */

#define IOTR0		0x1e80	/* 64bit */
#define IOTR1		0x1e88	/* 64bit */
#define IOTR2		0x1e90	/* 64bit */
#define IOTR3		0x1e98	/* 64bit */

#define TCTL		0x3000	/*  8bit */


#define D31IP		0x3100	/* 32bit */
#define D31IP_TTIP	24	/* Thermal Throttle Pin */
#define D31IP_SIP2	20	/* SATA Pin 2 */
#define D31IP_SMIP	12	/* SMBUS Pin */
#define D31IP_SIP	8	/* SATA Pin */
#define D30IP		0x3104	/* 32bit */
#define D30IP_PIP	0	/* PCI Bridge Pin */
#define D29IP		0x3108	/* 32bit */
#define D29IP_E1P	0	/* EHCI #1 Pin */
#define D28IP		0x310c	/* 32bit */
#define D28IP_P8IP	28	/* PCI Express Port 8 */
#define D28IP_P7IP	24	/* PCI Express Port 7 */
#define D28IP_P6IP	20	/* PCI Express Port 6 */
#define D28IP_P5IP	16	/* PCI Express Port 5 */
#define D28IP_P4IP	12	/* PCI Express Port 4 */
#define D28IP_P3IP	8	/* PCI Express Port 3 */
#define D28IP_P2IP	4	/* PCI Express Port 2 */
#define D28IP_P1IP	0	/* PCI Express Port 1 */
#define D27IP		0x3110	/* 32bit */
#define D27IP_ZIP	0	/* HD Audio Pin */
#define D26IP		0x3114	/* 32bit */
#define D26IP_E2P	0	/* EHCI #2 Pin */
#define D25IP		0x3118	/* 32bit */
#define D25IP_LIP	0	/* GbE LAN Pin */
#define D22IP		0x3124	/* 32bit */
#define D22IP_KTIP	12	/* KT Pin */
#define D22IP_IDERIP	8	/* IDE-R Pin */
#define D22IP_MEI2IP	4	/* MEI #2 Pin */
#define D22IP_MEI1IP	0	/* MEI #1 Pin */
#define D20IP		0x3128  /* 32bit */
#define D20IP_XHCIIP	0

#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define D26IR		0x314c	/* 16bit */
#define D25IR		0x3150	/* 16bit */
#define D23IR		0x3158	/* 16bit */
#define D22IR		0x315c	/* 16bit */
#define D21IR		0x3164	/* 16bit */
#define D20IR		0x3160	/* 16bit */
#define D19IR		0x3168	/* 16bit */

#define OIC		0x31ff	/*  8bit */

#define DIR_ROUTE(x, a, b, c, d) \
	(RCBA16(x) = (((d) << DIR_IDR) | ((c) << DIR_ICR) | \
		((b) << DIR_IBR) | ((a) << DIR_IAR)))

#define RC		0x3400	/* 32bit */
#define HPTC		0x3404	/* 32bit */
#define GCS		0x3410	/* 32bit */
#define BUC		0x3414	/* 32bit */
#define PCH_DISABLE_GBE		(1 << 5)
#define FD		0x3418	/* 32bit */
#define DISPBDF		0x3424  /* 16bit */
#define FD2		0x3428	/* 32bit */
#define CG		0x341c	/* 32bit */

/* Function Disable 1 RCBA 0x3418 */
#define PCH_DISABLE_ALWAYS	((1 << 0)|(1 << 26))
#define PCH_DISABLE_P2P		(1 << 1)
#define PCH_DISABLE_SATA1	(1 << 2)
#define PCH_DISABLE_SMBUS	(1 << 3)
#define PCH_DISABLE_HD_AUDIO	(1 << 4)
#define PCH_DISABLE_EHCI2	(1 << 13)
#define PCH_DISABLE_LPC		(1 << 14)
#define PCH_DISABLE_EHCI1	(1 << 15)
#define PCH_DISABLE_PCIE(x)	(1 << (16 + x))
#define PCH_DISABLE_THERMAL	(1 << 24)
#define PCH_DISABLE_SATA2	(1 << 25)
#define PCH_DISABLE_XHCI	(1 << 27)

/* Function Disable 2 RCBA 0x3428 */
#define PCH_DISABLE_KT		(1 << 4)
#define PCH_DISABLE_IDER	(1 << 3)
#define PCH_DISABLE_MEI2	(1 << 2)
#define PCH_DISABLE_MEI1	(1 << 1)
#define PCH_ENABLE_DBDF		(1 << 0)

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
#define FD_UHCI34	((1 << 10) | FD_UHCI4)
#define FD_UHCI234	((1 <<  9) | FD_UHCI3)
#define FD_UHCI1234	((1 <<  8) | FD_UHCI2)

#define FD_INTLAN	(1 <<  7)
#define FD_ACMOD	(1 <<  6)
#define FD_ACAUD	(1 <<  5)
#define FD_HDAUD	(1 <<  4)
#define FD_SMBUS	(1 <<  3)
#define FD_SATA		(1 <<  2)
#define FD_PATA		(1 <<  1)

#endif /* __ACPI__ */
#endif /* SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H */
