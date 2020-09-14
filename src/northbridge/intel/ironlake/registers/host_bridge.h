/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __IRONLAKE_REGISTERS_HOST_BRIDGE_H__
#define __IRONLAKE_REGISTERS_HOST_BRIDGE_H__

#define EPBAR		0x40
#define MCHBAR		0x48
#define GGC		0x52
#define DEVEN		0x54
#define  DEVEN_IGD	(1 << 3)
#define  DEVEN_PEG10	(1 << 1)
#define  DEVEN_HOST	(1 << 0)

#define DMIBAR		0x68

#define LAC		0x87 /* Legacy Access Control */

#define REMAPBASE	0x98
#define REMAPLIMIT	0x9a
#define TOM		0xa0
#define TOUUD		0xa2
#define IGD_BASE	0xa4
#define GTT_BASE	0xa8
#define TSEG		0xac /* TSEG base */
#define TOLUD		0xb0

#define SKPD		0xdc /* Scratchpad Data */

#define CAPID0		0xe0

#endif /* __IRONLAKE_REGISTERS_HOST_BRIDGE_H__ */
