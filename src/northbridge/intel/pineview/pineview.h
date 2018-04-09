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

#define BOOT_PATH_NORMAL	0
#define BOOT_PATH_RESET		1
#define BOOT_PATH_RESUME	2

#define SYSINFO_DIMM_NOT_POPULATED	0x00
#define SYSINFO_DIMM_X16SS		0x01
#define SYSINFO_DIMM_X16DS		0x02
#define SYSINFO_DIMM_X8DS		0x05
#define SYSINFO_DIMM_X8DDS		0x06

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

/* Device 0:1.0 PCI configuration space (PCI Express) */

#define BCTRL1		0x3e	/* 16bit */
#define PEGSTS		0x214	/* 32bit */

/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define GMADR		0x18
#define GTTADR		0x1c
#define BSM		0x5c

#define GPIO32(x) *((volatile u32 *)(DEFAULT_GPIOBASE + x))

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

enum fsb_clk {
	FSB_CLOCK_667MHz = 0,
	FSB_CLOCK_800MHz = 1,
};

enum mem_clk {
	MEM_CLOCK_667MHz = 0,
	MEM_CLOCK_800MHz = 1,
};

enum ddr {
	DDR2 = 2,
	DDR3 = 3,
};

enum chip_width { /* as in DDR3 spd */
	CHIP_WIDTH_x4	= 0,
	CHIP_WIDTH_x8	= 1,
	CHIP_WIDTH_x16	= 2,
	CHIP_WIDTH_x32	= 3,
};

enum chip_cap { /* as in DDR3 spd */
	CHIP_CAP_256M	= 0,
	CHIP_CAP_512M	= 1,
	CHIP_CAP_1G	= 2,
	CHIP_CAP_2G	= 3,
	CHIP_CAP_4G	= 4,
	CHIP_CAP_8G	= 5,
	CHIP_CAP_16G	= 6,
};

struct timings {
	unsigned int	CAS;
	enum fsb_clk	fsb_clock;
	enum mem_clk	mem_clock;
	unsigned int	tRAS;
	unsigned int	tRP;
	unsigned int	tRCD;
	unsigned int	tWR;
	unsigned int	tRFC;
	unsigned int	tWTR;
	unsigned int	tRRD;
	unsigned int	tRTP;
};

struct dimminfo {
	unsigned int	card_type; /* 0x0: unpopulated,
				      0xa - 0xf: raw card type A - F */
	u8		type;
	enum chip_width	width;
	enum chip_cap	chip_capacity;
	unsigned int	page_size; /* of whole DIMM in Bytes (4096 or 8192) */
	unsigned int	sides;
	unsigned int	banks;
	unsigned int	ranks;
	unsigned int	rows;
	unsigned int	cols;
	unsigned int	cas_latencies;
	unsigned int	tAAmin;
	unsigned int	tCKmin;
	unsigned int	tWR;
	unsigned int	tRP;
	unsigned int	tRCD;
	unsigned int	tRAS;
	unsigned int	rank_capacity_mb; /* per rank in Megabytes */
	u8		spd_data[256];
};

struct pllparam {
	u8 kcoarse[2][72];
	u8 pi[2][72];
	u8 dben[2][72];
	u8 dbsel[2][72];
	u8 clkdelay[2][72];
};

struct sysinfo {
	u8 maxpi;
	u8 pioffset;
	u8 pi[8];
	u16 coarsectrl;
	u16 coarsedelay;
	u16 mediumphase;
	u16 readptrdelay;

	int		txt_enabled;
	int		cores;
	int		boot_path;
	int		max_ddr2_mhz;
	int		max_ddr3_mt;
	int		max_fsb_mhz;
	int		max_render_mhz;
	int		enable_igd;
	int		enable_peg;
	u16		ggc;

	int		dimm_config[2];
	int		dimms_per_ch;
	int		spd_type;
	int		channel_capacity[2];
	struct timings	selected_timings;
	struct dimminfo	dimms[4];
	u8		spd_map[4];

	u8 nodll;
	u8 async;
	u8 dt0mode;
	u8 mvco4x;		/* 0 (8x) or 1 (4x) */
};

void pineview_early_initialization(void);
u32 decode_igd_memory_size(const u32 gms);
u32 decode_igd_gtt_size(const u32 gsm);
u8 decode_pciebar(u32 *const base, u32 *const len);

/* provided by mainboard code */
void setup_ich7_gpios(void);

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(unsigned long start, struct acpi_rsdp *rsdp);

#endif /* NORTHBRIDGE_INTEL_PINEVIEW_H */
