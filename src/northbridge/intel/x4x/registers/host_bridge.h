/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __X4X_REGISTERS_HOSTBRIDGE_H__
#define __X4X_REGISTERS_HOSTBRIDGE_H__

#define D0F0_EPBAR_LO	0x40
#define D0F0_EPBAR_HI	0x44
#define D0F0_MCHBAR_LO	0x48
#define D0F0_MCHBAR_HI	0x4c
#define D0F0_GGC	0x52
#define D0F0_DEVEN	0x54
#define  D0EN		(1 << 0)
#define  D1EN		(1 << 1)
#define  IGD0EN		(1 << 3)
#define  IGD1EN		(1 << 4)
#define  D3F0EN		(1 << 6)
#define  D3F1EN		(1 << 7)
#define  D3F2EN		(1 << 8)
#define  D3F3EN		(1 << 9)
#define  PEG1EN		(1 << 13)
#define  BOARD_DEVEN	(D0EN | D1EN | IGD0EN | IGD1EN | PEG1EN)
#define D0F0_PCIEXBAR_LO	0x60
#define D0F0_PCIEXBAR_HI	0x64
#define D0F0_DMIBAR_LO	0x68
#define D0F0_DMIBAR_HI	0x6c
#define D0F0_PAM(x)	(0x90 + (x)) /* 0-6 */
#define D0F0_REMAPBASE	0x98
#define D0F0_REMAPLIMIT	0x9a
#define D0F0_SMRAM	0x9d
#define D0F0_ESMRAMC	0x9e
#define D0F0_TOM	0xa0
#define D0F0_TOUUD	0xa2
#define D0F0_TOLUD	0xb0
#define D0F0_GBSM	0xa4
#define D0F0_BGSM	0xa8
#define D0F0_TSEG	0xac
#define D0F0_SKPD	0xdc /* Scratchpad Data */
#define D0F0_CAPID0	0xe0

#endif /* __X4X_REGISTERS_HOSTBRIDGE_H__ */
