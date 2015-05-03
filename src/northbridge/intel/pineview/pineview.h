/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef NORTHBRIDGE_INTEL_PINEVIEW_H
#define NORTHBRIDGE_INTEL_PINEVIEW_H

#include <northbridge/intel/pineview/iomap.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

/* Device 0:0.0 PCI configuration space (Host Bridge) */

#define EPBAR		0x40
#define MCHBAR		0x48
#define PCIEXBAR	0x60
#define DMIBAR		0x68
#define PMIOBAR		0x78

#define GGC		0x52			/* GMCH Graphics Control */

#define DEVEN		0x54			/* Device Enable */
#define  DEVEN_D0F0 (1 << 0)
#define  DEVEN_D1F0 (1 << 1)
#define  DEVEN_D2F0 (1 << 3)
#define  DEVEN_D2F1 (1 << 4)

#ifndef BOARD_DEVEN
#define BOARD_DEVEN ( DEVEN_D0F0 | DEVEN_D2F0 | DEVEN_D2F1 )
#endif /* BOARD_DEVEN */

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
#define ESMRAM		0x9e	/* Extended System Management RAM Control */

#define TOM		0xa0
#define TOUUD		0xa2
#define GBSM		0xa4
#define BGSM		0xa8
#define TSEGMB		0xac
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

/* Device 0:1.0 PCI configuration space (PCI Express) */

#define BCTRL1		0x3e	/* 16bit */
#define PEGSTS		0x214	/* 32bit */


/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define GMADR		0x18
#define GTTADR		0x1c
#define BSM		0x5c
#define GCFC		0xf0	/* Graphics Clock Frequency & Gating Control */


/*
 * MCHBAR
 */

#define MCHBAR8(x) *((volatile u8 *)(DEFAULT_MCHBAR + x))
#define MCHBAR16(x) *((volatile u16 *)(DEFAULT_MCHBAR + x))
#define MCHBAR32(x) *((volatile u32 *)(DEFAULT_MCHBAR + x))

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPBAR8(x) *((volatile u8 *)(DEFAULT_EPBAR + x))
#define EPBAR16(x) *((volatile u16 *)(DEFAULT_EPBAR + x))
#define EPBAR32(x) *((volatile u32 *)(DEFAULT_EPBAR + x))

/*
 * DMIBAR
 */

#define DMIBAR8(x) *((volatile u8 *)(DEFAULT_DMIBAR + x))
#define DMIBAR16(x) *((volatile u16 *)(DEFAULT_DMIBAR + x))
#define DMIBAR32(x) *((volatile u32 *)(DEFAULT_DMIBAR + x))

/* provided by mainboard code */
void setup_ich7_gpios(void);

#endif /* NORTHBRIDGE_INTEL_PINEVIEW_H */
