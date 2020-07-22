/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __IRONLAKE_HOSTBRIDGE_REGS_H__
#define __IRONLAKE_HOSTBRIDGE_REGS_H__

#define D0F0_EPBAR_LO		0x40
#define D0F0_EPBAR_HI		0x44
#define D0F0_MCHBAR_LO		0x48
#define D0F0_MCHBAR_HI		0x4c
#define D0F0_GGC		0x52
#define D0F0_DEVEN		0x54
#define  DEVEN_IGD		(1 << 3)
#define  DEVEN_PEG10		(1 << 1)
#define  DEVEN_HOST		(1 << 0)
#define D0F0_PCIEXBAR_LO	0x60
#define D0F0_PCIEXBAR_HI	0x64
#define D0F0_DMIBAR_LO		0x68
#define D0F0_DMIBAR_HI		0x6c
#define D0F0_PMBASE		0x78

#define D0F0_REMAPBASE		0x98
#define D0F0_REMAPLIMIT		0x9a
#define D0F0_TOM		0xa0
#define D0F0_TOUUD		0xa2
#define D0F0_IGD_BASE		0xa4
#define D0F0_GTT_BASE		0xa8
#define D0F0_TOLUD		0xb0
#define D0F0_SKPD		0xdc /* Scratchpad Data */

#define D0F0_CAPID0		0xe0

#define TSEG			0xac /* TSEG base */

/* FIXME: Deduplicate these registers */
#define EPBAR		0x40
#define MCHBAR		0x48
#define PCIEXBAR	0x60
#define DMIBAR		0x68
#define X60BAR		0x60

#define LAC		0x87	/* Legacy Access Control */

#define SKPAD		0xdc	/* Scratchpad Data */

#endif /* __IRONLAKE_HOSTBRIDGE_REGS_H__ */
