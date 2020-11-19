/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SANDYBRIDGE_REGISTERS_HOST_BRIDGE_H__
#define __SANDYBRIDGE_REGISTERS_HOST_BRIDGE_H__

#define EPBAR		0x40
#define MCHBAR		0x48

#define GGC		0x50	/* GMCH Graphics Control */
#define DEVEN		0x54	/* Device Enable */
#define  DEVEN_D7EN	(1 << 14)
#define  DEVEN_PEG60	(1 << 13)
#define  DEVEN_D4EN	(1 <<  7)
#define  DEVEN_IGD	(1 <<  4)
#define  DEVEN_PEG10	(1 <<  3)
#define  DEVEN_PEG11	(1 <<  2)
#define  DEVEN_PEG12	(1 <<  1)
#define  DEVEN_HOST	(1 <<  0)

#define PAVPC		0x58	/* Protected Audio Video Path Control */
#define DPR		0x5c	/* DMA Protected Range */

#define PCIEXBAR	0x60
#define DMIBAR		0x68

#define MESEG_BASE	0x70
#define MESEG_MASK	0x78
#define  MELCK		(1 << 10)	/* ME Range Lock */
#define  ME_STLEN_EN	(1 << 11)	/* ME Stolen Memory Enable */

#define PAM0		0x80
#define PAM1		0x81
#define PAM2		0x82
#define PAM3		0x83
#define PAM4		0x84
#define PAM5		0x85
#define PAM6		0x86

#define LAC		0x87	/* Legacy Access Control */
#define SMRAM		0x88	/* System Management RAM Control */

#define REMAPBASE	0x90
#define REMAPLIMIT	0x98
#define TOM		0xa0
#define TOUUD		0xa8	/* Top of Upper Usable DRAM */
#define BDSM		0xb0	/* Base Data of Stolen Memory */
#define BGSM		0xb4	/* Base GTT Stolen Memory */
#define TSEGMB		0xb8	/* TSEG Memory Base */
#define TOLUD		0xbc	/* Top of Low Used Memory */

#define CAPID0_A	0xe4	/* Capabilities Register A */
#define  CAPID_ECCDIS	(1 << 25)
#define  CAPID_DDPCD	(1 << 14)
#define  CAPID_PDCD	(1 << 12)
#define  CAPID_WRTVREF	(1 <<  1)
#define  CAPID_DDRSZ(x)	(((x) >> 19) & 0x3)

#define CAPID0_B	0xe8	/* Capabilities Register B */

#define SKPAD		0xdc	/* Scratchpad Data */

#define DIDOR		0xf3	/* Device ID override, for debug and samples only */

#endif /* __SANDYBRIDGE_REGISTERS_HOST_BRIDGE_H__ */
