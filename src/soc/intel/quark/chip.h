/* SPDX-License-Identifier: GPL-2.0-only */

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

	uint32_t FspReservedMemoryLength; /* FSP reserved memory in bytes */

	uint32_t Flags;         /* Bitmap of MRC_FLAG_XXX defs above */
	uint32_t tRAS;          /* ACT to PRE command period in picoseconds */

	/* Delay from start of internal write transaction to internal read
	 * command in picoseconds
	 */
	uint32_t tWTR;

	/* ACT to ACT command period (JESD79 specific to page size 1K/2K) in
	 * picoseconds
	 */
	uint32_t tRRD;

	/* Four activate window (JESD79 specific to page size 1K/2K) in
	 * picoseconds
	 */
	uint32_t tFAW;
	uint8_t  DramWidth;     /* 0=x8, 1=x16, others=RESERVED */

	/* 0=DDRFREQ_800, 1=DDRFREQ_1066, others=RESERVED. Only 533MHz SKU
	 * support 1066 memory
	 */
	uint8_t  DramSpeed;
	uint8_t  DramType;      /* 0=DDR3,1=DDR3L, others=RESERVED */

	/* bit[0] RANK0_EN, bit[1] RANK1_EN, others=RESERVED */
	uint8_t  RankMask;
	uint8_t  ChanMask;      /* bit[0] CHAN0_EN, others=RESERVED */
	uint8_t  ChanWidth;     /* 1=x16, others=RESERVED */

	/* 0, 1, 2 (mode 2 forced if ecc enabled), others=RESERVED */
	uint8_t  AddrMode;

	/* 1=1.95us, 2=3.9us, 3=7.8us, others=RESERVED. REFRESH_RATE */
	uint8_t  SrInt;
	uint8_t  SrTemp;        /* 0=normal, 1=extended, others=RESERVED */

	/* 0=34ohm, 1=40ohm, others=RESERVED. RON_VALUE Select MRS1.DIC driver
	 * impedance control.
	 */
	uint8_t  DramRonVal;
	uint8_t  DramRttNomVal; /* 0=40ohm, 1=60ohm, 2=120ohm, others=RSVD */
	uint8_t  DramRttWrVal;  /* 0=off others=RESERVED */

	/* 0=off, 1=60ohm, 2=120ohm, 3=180ohm, others=RESERVED */
	uint8_t  SocRdOdtVal;
	uint8_t  SocWrRonVal;   /* 0=27ohm, 1=32ohm, 2=40ohm, others=RESERVED */
	uint8_t  SocWrSlewRate; /* 0=2.5V/ns, 1=4V/ns, others=RESERVED */

	/* 0=512Mb, 1=1Gb, 2=2Gb, 3=4Gb, others=RESERVED */
	uint8_t  DramDensity;
	uint8_t  tCL;           /* DRAM CAS Latency in clocks */

	/* ECC scrub interval in milliseconds 1..255 (0 works as feature
	 * disable)
	 */
	uint8_t EccScrubInterval;

	/* Number of 32B blocks read for ECC scrub 2..16 */
	uint8_t EccScrubBlkSize;

	uint8_t  SmmTsegSize;   /* SMM size in MiB */
};

#endif
