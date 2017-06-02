/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef SOC_SKYLAKE_SYSTEMAGENT_H
#define SOC_SKYLAKE_SYSTEMAGENT_H

#include <intelblocks/systemagent.h>
#include <soc/iomap.h>

#define SA_IGD_OPROM_VENDEV	0x80860406

/* Device 0:0.0 PCI configuration space */

#define EPBAR		0x40
#define PCIEXBAR	0x60
#define DMIBAR		0x68
#define GGC		0x50	/* GMCH Graphics Control */
#define DEVEN		0x54	/* Device Enable */
#define  DEVEN_D7EN	(1 << 14)
#define  DEVEN_D4EN	(1 << 7)
#define  DEVEN_D3EN	(1 << 5)
#define  DEVEN_D2EN	(1 << 4)
#define  DEVEN_D1F0EN	(1 << 3)
#define  DEVEN_D1F1EN	(1 << 2)
#define  DEVEN_D1F2EN	(1 << 1)
#define  DEVEN_D0EN	(1 << 0)
#define DPR		0x5c
#define  DPR_EPM	(1 << 2)
#define  DPR_PRS	(1 << 1)
#define  DPR_SIZE_MASK	0xff0

#define PAM0		0x80
#define PAM1		0x81
#define PAM2		0x82
#define PAM3		0x83
#define PAM4		0x84
#define PAM5		0x85
#define PAM6		0x86

#define SMRAM		0x88	/* System Management RAM Control */
#define  D_OPEN		(1 << 6)
#define  D_CLS		(1 << 5)
#define  D_LCK		(1 << 4)
#define  G_SMRAME	(1 << 3)
#define  C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

#define MESEG_BASE	0x70	/* Management Engine Base. */
#define MESEG_LIMIT	0x78	/* Management Engine Limit. */
#define REMAPBASE	0x90	/* Remap base. */
#define REMAPLIMIT	0x98	/* Remap limit. */
#define TOM		0xa0	/* Top of DRAM in memory controller space. */
#define SKPAD		0xdc	/* Scratchpad Data */

/* MCHBAR */

#define MCHBAR8(x)	(*(volatile u8 *)(MCH_BASE_ADDRESS + x))
#define MCHBAR16(x)	(*(volatile u16 *)(MCH_BASE_ADDRESS + x))
#define MCHBAR32(x)	(*(volatile u32 *)(MCH_BASE_ADDRESS + x))

#define MCHBAR_PEI_VERSION	0x5034
#define BIOS_RESET_CPL		0x5da8
#define EDRAMBAR		0x5408
#define MCH_PAIR		0x5418
#define GDXCBAR			0x5420

#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4
#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

/* PCODE MMIO communications live in the MCHBAR. */
#define BIOS_MAILBOX_INTERFACE			0x5da4
#define  MAILBOX_RUN_BUSY			(1 << 31)
/* Errors are returned back in bits 7:0. */
#define  MAILBOX_BIOS_ERROR_NONE		0
#define  MAILBOX_BIOS_ERROR_INVALID_COMMAND	1
#define  MAILBOX_BIOS_ERROR_TIMEOUT		2
#define  MAILBOX_BIOS_ERROR_ILLEGAL_DATA	3
#define  MAILBOX_BIOS_ERROR_RESERVED		4
#define  MAILBOX_BIOS_ERROR_ILLEGAL_VR_ID	5
#define  MAILBOX_BIOS_ERROR_VR_INTERFACE_LOCKED	6
#define  MAILBOX_BIOS_ERROR_VR_ERROR		7
/* Data is passed through bits 31:0 of the data register. */
#define BIOS_MAILBOX_DATA			0x5da0

/* System Agent identification */
u8 systemagent_revision(void);

/* Top of 32bit usable memory */
u32 top_of_32bit_ram(void);

#endif
