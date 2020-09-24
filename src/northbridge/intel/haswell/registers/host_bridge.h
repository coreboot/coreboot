/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __HASWELL_REGISTERS_HOST_BRIDGE_H__
#define __HASWELL_REGISTERS_HOST_BRIDGE_H__

#define EPBAR		0x40
#define MCHBAR		0x48

#define GGC		0x50	/* GMCH Graphics Control */
#define  GGC_DISABLE_VGA_IO_DECODE	(1 << 1)
#define  GGC_IGD_MEM_IN_32MB_UNITS(x)	(((x) & 0x1f) << 3)
#define  GGC_GTT_0MB			(0 << 8)
#define  GGC_GTT_1MB			(1 << 8)
#define  GGC_GTT_2MB			(2 << 8)

#define DEVEN		0x54	/* Device Enable */
#define  DEVEN_D7EN	(1 << 14)
#define  DEVEN_D4EN	(1 << 7)
#define  DEVEN_D3EN	(1 << 5)
#define  DEVEN_D2EN	(1 << 4)
#define  DEVEN_D1F0EN	(1 << 3)
#define  DEVEN_D1F1EN	(1 << 2)
#define  DEVEN_D1F2EN	(1 << 1)
#define  DEVEN_D0EN	(1 << 0)

#define PAVPC		0x58
#define DPR		0x5c
#define  DPR_EPM	(1 << 2)
#define  DPR_PRS	(1 << 1)
#define  DPR_SIZE_MASK	0xff0

#define PCIEXBAR	0x60
#define DMIBAR		0x68

#define MESEG_BASE	0x70	/* Management Engine Base */
#define MESEG_LIMIT	0x78	/* Management Engine Limit */

#define PAM0		0x80
#define PAM1		0x81
#define PAM2		0x82
#define PAM3		0x83
#define PAM4		0x84
#define PAM5		0x85
#define PAM6		0x86

#define LAC		0x87	/* Legacy Access Control */
#define SMRAM		0x88	/* System Management RAM Control */
#define  D_OPEN		(1 << 6)
#define  D_CLS		(1 << 5)
#define  D_LCK		(1 << 4)
#define  G_SMRAME	(1 << 3)
#define  C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

#define REMAPBASE	0x90	/* Remap base */
#define REMAPLIMIT	0x98	/* Remap limit */
#define TOM		0xa0	/* Top of DRAM in memory controller space */
#define TOUUD		0xa8	/* Top of Upper Usable DRAM */
#define BDSM		0xb0	/* Base Data Stolen Memory */
#define BGSM		0xb4	/* Base GTT Stolen Memory */
#define TSEG		0xb8	/* TSEG base */
#define TOLUD		0xbc	/* Top of Low Used Memory */

#define SKPAD		0xdc	/* Scratchpad Data */

#define CAPID0_A	0xe4
#define  CAPID_ECCDIS	(1 << 25)
#define  VTD_DISABLE	(1 << 23)
#define  CAPID_DDPCD	(1 << 14)
#define  CAPID_PDCD	(1 << 12)
#define  CAPID_DDRSZ(x)	(((x) >> 19) & 0x3)

#define CAPID0_B	0xe8

#endif /* __HASWELL_REGISTERS_HOST_BRIDGE_H__ */
