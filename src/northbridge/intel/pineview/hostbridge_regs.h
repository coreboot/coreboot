/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __PINEVIEW_HOSTBRIDGE_REGS_H__
#define __PINEVIEW_HOSTBRIDGE_REGS_H__

#define EPBAR		0x40
#define MCHBAR		0x48

#define GGC		0x52	/* GMCH Graphics Control */

#define DEVEN		0x54	/* Device Enable */
#define  DEVEN_D0F0 (1 << 0)
#define  DEVEN_D1F0 (1 << 1)
#define  DEVEN_D2F0 (1 << 3)
#define  DEVEN_D2F1 (1 << 4)

#ifndef BOARD_DEVEN
#define BOARD_DEVEN (DEVEN_D0F0 | DEVEN_D2F0 | DEVEN_D2F1)
#endif /* BOARD_DEVEN */

#define PCIEXBAR	0x60
#define DMIBAR		0x68
#define PMIOBAR		0x78

#define PAM0		0x90
#define PAM1		0x91
#define PAM2		0x92
#define PAM3		0x93
#define PAM4		0x94
#define PAM5		0x95
#define PAM6		0x96

#define LAC		0x97	/* Legacy Access Control */
#define REMAPBASE	0x98
#define REMAPLIMIT	0x9a
#define SMRAM		0x9d	/* System Management RAM Control */
#define ESMRAMC		0x9e	/* Extended System Management RAM Control */

#define TOM		0xa0
#define TOUUD		0xa2
#define GBSM		0xa4
#define BGSM		0xa8
#define TSEG		0xac
#define TOLUD		0xb0	/* Top of Low Used Memory */
#define ERRSTS		0xc8
#define ERRCMD		0xca
#define SMICMD		0xcc
#define SCICMD		0xce
#define CGDIS		0xd8
#define SKPAD		0xdc	/* Scratchpad Data */
#define CAPID0		0xe0
#define DEV0T		0xf0
#define MSLCK		0xf4
#define MID0		0xf8
#define DEBUP0		0xfc

#endif /* __PINEVIEW_HOSTBRIDGE_REGS_H__ */
