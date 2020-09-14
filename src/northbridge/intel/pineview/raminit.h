/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef PINEVIEW_RAMINIT_H
#define PINEVIEW_RAMINIT_H

#define SYSINFO_DIMM_NOT_POPULATED	0x00
#define SYSINFO_DIMM_X16SS		0x01
#define SYSINFO_DIMM_X16DS		0x02
#define SYSINFO_DIMM_X8DS		0x05
#define SYSINFO_DIMM_X8DDS		0x06

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

void sdram_initialize(int boot_path, const u8 *sdram_addresses);

#endif /* PINEVIEW_RAMINIT_H */
