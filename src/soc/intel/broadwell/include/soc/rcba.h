/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_RCBA_H_
#define _BROADWELL_RCBA_H_

#include <soc/iomap.h>

#define RCBA8(x)	*((volatile u8 *)(RCBA_BASE_ADDRESS + x))
#define RCBA16(x)	*((volatile u16 *)(RCBA_BASE_ADDRESS + x))
#define RCBA32(x)	*((volatile u32 *)(RCBA_BASE_ADDRESS + x))

#define RCBA_AND_OR(bits, x, and, or) \
	RCBA##bits(x) = ((RCBA##bits(x) & (and)) | (or))
#define RCBA8_AND_OR(x, and, or)  RCBA_AND_OR(8, x, and, or)
#define RCBA16_AND_OR(x, and, or) RCBA_AND_OR(16, x, and, or)
#define RCBA32_AND_OR(x, and, or) RCBA_AND_OR(32, x, and, or)
#define RCBA32_OR(x, or) RCBA_AND_OR(32, x, ~0UL, or)

#define RPC		0x0400	/* 32bit */
#define RPFN		0x0404	/* 32bit */

/* Root Port configuration space hide */
#define RPFN_HIDE(port)         (1 << (((port) * 4) + 3))
/* Get the function number assigned to a Root Port */
#define RPFN_FNGET(reg, port)   (((reg) >> ((port) * 4)) & 7)
/* Set the function number for a Root Port */
#define RPFN_FNSET(port, func)  (((func) & 7) << ((port) * 4))
/* Root Port function number mask */
#define RPFN_FNMASK(port)       (7 << ((port) * 4))

#define NOINT		0
#define INTA		1
#define INTB		2
#define INTC		3
#define INTD		4

#define DIR_IDR		12	/* Interrupt D Pin Offset */
#define DIR_ICR		8	/* Interrupt C Pin Offset */
#define DIR_IBR		4	/* Interrupt B Pin Offset */
#define DIR_IAR		0	/* Interrupt A Pin Offset */

#define PIRQA		0
#define PIRQB		1
#define PIRQC		2
#define PIRQD		3
#define PIRQE		4
#define PIRQF		5
#define PIRQG		6
#define PIRQH		7

#define LCAP		0x21a4

/* IO Buffer Programming */
#define IOBPIRI		0x2330
#define IOBPD		0x2334
#define IOBPS		0x2338
#define  IOBPS_READY	0x0001
#define  IOBPS_TX_MASK	0x0006
#define  IOBPS_MASK     0xff00
#define  IOBPS_READ     0x0600
#define  IOBPS_WRITE	0x0700
#define IOBPU		0x233a
#define  IOBPU_MAGIC	0xf000
#define  IOBP_PCICFG_READ	0x0400
#define  IOBP_PCICFG_WRITE	0x0500

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
#define D20IP		0x3128	/* 32bit */
#define D20IP_XHCI	0	/* XHCI Pin */
#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define D26IR		0x314c	/* 16bit */
#define D25IR		0x3150	/* 16bit */
#define D23IR		0x3158	/* 16bit */
#define D22IR		0x315c	/* 16bit */
#define D20IR		0x3160	/* 16bit */
#define D21IR		0x3164	/* 16bit */
#define D19IR		0x3168	/* 16bit */
#define ACPIIRQEN	0x31e0	/* 32bit */
#define OIC		0x31fe	/* 16bit */
#define DEEP_S3_POL	0x3328	/* 32bit */
#define  DEEP_S3_EN_AC		(1 << 0)
#define  DEEP_S3_EN_DC		(1 << 1)
#define DEEP_S5_POL	0x3330	/* 32bit */
#define  DEEP_S5_EN_AC		(1 << 14)
#define  DEEP_S5_EN_DC		(1 << 15)
#define DEEP_SX_CONFIG	0x3334	/* 32bit */
#define  DEEP_SX_WAKE_PIN_EN	(1 << 2)
#define  DEEP_SX_ACPRESENT_PD	(1 << 1)
#define  DEEP_SX_GP27_PIN_EN	(1 << 0)
#define PMSYNC_CONFIG	0x33c4	/* 32bit */
#define PMSYNC_CONFIG2	0x33cc	/* 32bit */
#define SOFT_RESET_CTRL 0x38f4
#define SOFT_RESET_DATA 0x38f8

#define DIR_ROUTE(a, b, c, d) \
	(((d) << DIR_IDR) | ((c) << DIR_ICR) | \
	((b) << DIR_IBR) | ((a) << DIR_IAR))

#define RC		0x3400	/* 32bit */
#define HPTC		0x3404	/* 32bit */
#define GCS		0x3410	/* 32bit */
#define BUC		0x3414	/* 32bit */
#define PCH_DISABLE_GBE		(1 << 5)
#define FD		0x3418	/* 32bit */
#define FDSW		0x3420	/* 8bit */
#define DISPBDF		0x3424  /* 16bit */
#define FD2		0x3428	/* 32bit */
#define CG		0x341c	/* 32bit */

/* Function Disable 1 RCBA 0x3418 */
#define PCH_DISABLE_ALWAYS	(1 << 0)
#define PCH_DISABLE_ADSPD	(1 << 1)
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

#endif
