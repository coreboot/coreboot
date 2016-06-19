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

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <stdint.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

///
/// MRC Flags bits
///
#define MRC_FLAG_ECC_EN		BIT0
#define MRC_FLAG_SCRAMBLE_EN	BIT1
#define MRC_FLAG_MEMTEST_EN	BIT2

/* 0b DDR "fly-by" topology else 1b DDR "tree" topology */
#define MRC_FLAG_TOP_TREE_EN	BIT3

/* If set ODR signal is asserted to DRAM devices on writes */
#define MRC_FLAG_WR_ODT_EN	BIT4

struct soc_intel_quark_config {
	/*
	 * MemoryInit:
	 *
	 * The following fields come from FspUpdVpd.h and are defined as PCDs
         * for the FSP binary.  Data for these fields comes from the board's
	 * devicetree.cb file which gets processed into static.c and then
	 * built into the coreboot image.  The fields below contain retain
	 * the FSP PCD field name.
	 */

	UINT32 FspReservedMemoryLength; /* FSP reserved memory in bytes */

	UINT32 Flags;         /* Bitmap of MRC_FLAG_XXX defs above */
	UINT32 tRAS;          /* ACT to PRE command period in picoseconds */

	/* Delay from start of internal write transaction to internal read
	 * command in picoseconds
	 */
	UINT32 tWTR;

	/* ACT to ACT command period (JESD79 specific to page size 1K/2K) in
	 * picoseconds
	 */
	UINT32 tRRD;

	/* Four activate window (JESD79 specific to page size 1K/2K) in
	 * picoseconds
	 */
	UINT32 tFAW;
	UINT8  DramWidth;     /* 0=x8, 1=x16, others=RESERVED */

	/* 0=DDRFREQ_800, 1=DDRFREQ_1066, others=RESERVED. Only 533MHz SKU
	 * support 1066 memory
	 */
	UINT8  DramSpeed;
	UINT8  DramType;      /* 0=DDR3,1=DDR3L, others=RESERVED */

	/* bit[0] RANK0_EN, bit[1] RANK1_EN, others=RESERVED */
	UINT8  RankMask;
	UINT8  ChanMask;      /* bit[0] CHAN0_EN, others=RESERVED */
	UINT8  ChanWidth;     /* 1=x16, others=RESERVED */

	/* 0, 1, 2 (mode 2 forced if ecc enabled), others=RESERVED */
	UINT8  AddrMode;

	/* 1=1.95us, 2=3.9us, 3=7.8us, others=RESERVED. REFRESH_RATE */
	UINT8  SrInt;
	UINT8  SrTemp;        /* 0=normal, 1=extended, others=RESERVED */

	/* 0=34ohm, 1=40ohm, others=RESERVED. RON_VALUE Select MRS1.DIC driver
	 * impedance control.
	 */
	UINT8  DramRonVal;
	UINT8  DramRttNomVal; /* 0=40ohm, 1=60ohm, 2=120ohm, others=RESERVED */
	UINT8  DramRttWrVal;  /* 0=off others=RESERVED */

	/* 0=off, 1=60ohm, 2=120ohm, 3=180ohm, others=RESERVED */
	UINT8  SocRdOdtVal;
	UINT8  SocWrRonVal;   /* 0=27ohm, 1=32ohm, 2=40ohm, others=RESERVED */
	UINT8  SocWrSlewRate; /* 0=2.5V/ns, 1=4V/ns, others=RESERVED */

	/* 0=512Mb, 1=1Gb, 2=2Gb, 3=4Gb, others=RESERVED */
	UINT8  DramDensity;
	UINT8  tCL;           /* DRAM CAS Latency in clocks */

	/* ECC scrub interval in miliseconds 1..255 (0 works as feature
	 * disable)
	 */
	UINT8 EccScrubInterval;

	/* Number of 32B blocks read for ECC scrub 2..16 */
	UINT8 EccScrubBlkSize;

	UINT8  SmmTsegSize;   /* SMM size in MiB */
};

extern struct chip_operations soc_ops;

#endif
