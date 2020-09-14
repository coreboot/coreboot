/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef NORTHBRIDGE_INTEL_PINEVIEW_H
#define NORTHBRIDGE_INTEL_PINEVIEW_H

#include <northbridge/intel/pineview/memmap.h>
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
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

#include "hostbridge_regs.h"

/* Device 0:1.0 PCI configuration space (PCI Express) */

#define PEGSTS		0x214	/* 32 bits */

/* Device 0:2.0 PCI configuration space (Integrated Graphics Device) */
#define GMCH_IGD	PCI_DEV(0, 2, 0)

#define GMADR		0x18
#define GTTADR		0x1c
#define BSM		0x5c

#define GPIO32(x) *((volatile u32 *)(DEFAULT_GPIOBASE + x))

/*
 * MCHBAR
 */

#define MCHBAR8(x)  (*((volatile u8  *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR16(x) (*((volatile u16 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR32(x) (*((volatile u32 *)(DEFAULT_MCHBAR +  x)))	/* FIXME: causes changes */
#define MCHBAR8_AND(x,  and) (MCHBAR8(x)  = MCHBAR8(x)  & (and))
#define MCHBAR16_AND(x, and) (MCHBAR16(x) = MCHBAR16(x) & (and))
#define MCHBAR32_AND(x, and) (MCHBAR32(x) = MCHBAR32(x) & (and))
#define MCHBAR8_OR(x,   or)  (MCHBAR8(x)  = MCHBAR8(x)  | (or))
#define MCHBAR16_OR(x,  or)  (MCHBAR16(x) = MCHBAR16(x) | (or))
#define MCHBAR32_OR(x,  or)  (MCHBAR32(x) = MCHBAR32(x) | (or))
#define MCHBAR8_AND_OR(x,  and, or) (MCHBAR8(x)  = (MCHBAR8(x)  & (and)) | (or))
#define MCHBAR16_AND_OR(x, and, or) (MCHBAR16(x) = (MCHBAR16(x) & (and)) | (or))
#define MCHBAR32_AND_OR(x, and, or) (MCHBAR32(x) = (MCHBAR32(x) & (and)) | (or))

/* As there are many registers, define them on a separate file */

#include "mchbar_regs.h"

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPBAR8(x)  *((volatile u8  *)(DEFAULT_EPBAR + (x)))
#define EPBAR16(x) *((volatile u16 *)(DEFAULT_EPBAR + (x)))
#define EPBAR32(x) *((volatile u32 *)(DEFAULT_EPBAR + (x)))

/*
 * DMIBAR
 */

#define DMIBAR8(x)  *((volatile u8  *)(DEFAULT_DMIBAR + (x)))
#define DMIBAR16(x) *((volatile u16 *)(DEFAULT_DMIBAR + (x)))
#define DMIBAR32(x) *((volatile u32 *)(DEFAULT_DMIBAR + (x)))

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

void pineview_early_init(void);
u32 decode_igd_memory_size(const u32 gms);
u32 decode_igd_gtt_size(const u32 gsm);
int decode_pcie_bar(u32 *const base, u32 *const len);

/* Mainboard romstage callback functions */
void get_mb_spd_addrmap(u8 *spd_addr_map);
void mb_pirq_setup(void); /* optional */

#endif /* NORTHBRIDGE_INTEL_PINEVIEW_H */
