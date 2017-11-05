/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 *               2012 secunet Security Networks AG
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
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

#ifndef __NORTHBRIDGE_INTEL_X4X_H__
#define __NORTHBRIDGE_INTEL_X4X_H__

/*
 * D0:F0
 */
#define D0F0_EPBAR_LO 0x40
#define D0F0_EPBAR_HI 0x44
#define D0F0_MCHBAR_LO 0x48
#define D0F0_MCHBAR_HI 0x4c
#define D0F0_GGC 0x52
#define D0F0_DEVEN 0x54
#define  D0EN		(1 << 0)
#define  D1EN		(1 << 1)
#define  IGD0EN		(1 << 3)
#define  IGD1EN		(1 << 4)
#define  D3F0EN		(1 << 6)
#define  D3F1EN		(1 << 7)
#define  D3F2EN		(1 << 8)
#define  D3F3EN		(1 << 9)
#define  PEG1EN		(1 << 13)
#define  BOARD_DEVEN	(D0EN | D1EN | IGD0EN | IGD1EN | PEG1EN)
#define D0F0_PCIEXBAR_LO 0x60
#define D0F0_PCIEXBAR_HI 0x64
#define D0F0_DMIBAR_LO 0x68
#define D0F0_DMIBAR_HI 0x6c
#define D0F0_PAM(x) (0x90+(x)) /* 0-6*/
#define D0F0_REMAPBASE 0x98
#define D0F0_REMAPLIMIT 0x9a
#define D0F0_SMRAM 0x9d
#define D0F0_ESMRAMC 0x9e
#define D0F0_TOM 0xa0
#define D0F0_TOUUD 0xa2
#define D0F0_TOLUD 0xb0
#define D0F0_GBSM 0xa4
#define D0F0_BGSM 0xa8
#define D0F0_TSEG 0xac
#define D0F0_SKPD 0xdc /* Scratchpad Data */
#define D0F0_CAPID0 0xe0

/*
 * D1:F0 PEG
 */
#define PEG_CAP 0xa2
#define SLOTCAP 0xb4
#define PEGLC 0xec
#define D1F0_VCCAP 0x104
#define D1F0_VC0RCTL 0x114

/*
 * Graphics frequencies
 */
#define GCFGC_PCIDEV		PCI_DEV(0, 2, 0)
#define GCFGC_OFFSET		0xf0
#define GCFGC_CR_SHIFT		0
#define GCFGC_CR_MASK		(0xf << GCFGC_CR_SHIFT)
#define GCFGC_CS_SHIFT		8
#define GCFGC_CS_MASK		(0xf << GCFGC_CS_SHIFT)
#define GCFGC_CD_SHIFT		12
#define GCFGC_CD_MASK		(0x1 << GCFGC_CD_SHIFT)
#define GCFGC_UPDATE_SHIFT	5
#define GCFGC_UPDATE		(0x1 << GCFGC_UPDATE_SHIFT)

/*
 * MCHBAR
 */

#define MCHBAR8(x) (*((volatile u8 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR16(x) (*((volatile u16 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR32(x) (*((volatile u32 *)(DEFAULT_MCHBAR + (x))))

#define CHDECMISC	0x111
#define STACKED_MEM	(1 << 1)

#define C0DRB0		0x200
#define C0DRB1		0x202
#define C0DRB2		0x204
#define C0DRB3		0x206
#define C0DRA01		0x208
#define C0DRA23		0x20a
#define C0CKECTRL	0x260

#define C1DRB0		0x600
#define C1DRB1		0x602
#define C1DRB2		0x604
#define C1DRB3		0x606
#define C1DRA01		0x608
#define C1DRA23		0x60a
#define C1CKECTRL	0x660

#define PMSTS_MCHBAR		0x0f14	/* Self refresh channel status */
#define PMSTS_WARM_RESET	(1 << 8)
#define PMSTS_BOTH_SELFREFRESH	(3 << 0)

#define CLKCFG_MCHBAR		0x0c00
#define CLKCFG_FSBCLK_SHIFT	0
#define CLKCFG_FSBCLK_MASK	(7 << CLKCFG_FSBCLK_SHIFT)
#define CLKCFG_MEMCLK_SHIFT	4
#define CLKCFG_MEMCLK_MASK	(7 << CLKCFG_MEMCLK_SHIFT)
#define CLKCFG_UPDATE		(1 << 12)

#define SSKPD_MCHBAR		0x0c20 /* 64 bit */

/*
 * DMIBAR
 */

#define DMIBAR8(x) (*((volatile u8 *)(DEFAULT_DMIBAR + (x))))
#define DMIBAR16(x) (*((volatile u16 *)(DEFAULT_DMIBAR + (x))))
#define DMIBAR32(x) (*((volatile u32 *)(DEFAULT_DMIBAR + (x))))

#define DMIVC0RCTL 0x14
#define DMIVC1RCTL 0x20
#define DMIVC1RSTS 0x26
#define DMIESD  0x44
#define DMILE1D 0x50
#define DMILE1A 0x58
#define DMILE2D 0x60
#define DMILE2A 0x68

/*
 * EPBAR
 */

#define EPBAR8(x) (*((volatile u8 *)(DEFAULT_EPBAR + (x))))
#define EPBAR16(x) (*((volatile u16 *)(DEFAULT_EPBAR + (x))))
#define EPBAR32(x) (*((volatile u32 *)(DEFAULT_EPBAR + (x))))

#define EPESD 0x44
#define EPLE1D 0x50
#define EPLE1A 0x58
#define EPLE2D 0x60

#define NOP_CMD 0x2
#define PRECHARGE_CMD 0x4
#define MRS_CMD 0x6
#define EMRS_CMD 0x8
#define EMRS1_CMD (EMRS_CMD | 0x10)
#define EMRS2_CMD (EMRS_CMD | 0x20)
#define EMRS3_CMD (EMRS_CMD | 0x30)
#define ZQCAL_CMD 0xa
#define CBR_CMD 0xc
#define NORMALOP_CMD 0xe

#define TOTAL_CHANNELS 2
#define TOTAL_DIMMS 4
#define TOTAL_BYTELANES 8
#define DIMMS_PER_CHANNEL (TOTAL_DIMMS / TOTAL_CHANNELS)
#define RAW_CARD_UNPOPULATED 0xff
#define RAW_CARD_POPULATED 0

#define DIMM_IS_POPULATED(dimms, idx) (dimms[idx].card_type != RAW_CARD_UNPOPULATED)
#define IF_DIMM_POPULATED(dimms, idx) if (dimms[idx].card_type != RAW_CARD_UNPOPULATED)
#define ONLY_DIMMA_IS_POPULATED(dimms, ch) ( \
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 0 : 2) && \
	!DIMM_IS_POPULATED(dimms, (ch == 0) ? 1 : 3)))
#define ONLY_DIMMB_IS_POPULATED(dimms, ch) ( \
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 1 : 3) && \
	!DIMM_IS_POPULATED(dimms, (ch == 0) ? 0 : 2)))
#define BOTH_DIMMS_ARE_POPULATED(dimms, ch) ( \
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 0 : 2) && \
	(DIMM_IS_POPULATED(dimms, (ch == 0) ? 1 : 3))))
#define FOR_EACH_DIMM(idx) \
	for (idx = 0; idx < TOTAL_DIMMS; ++idx)
#define FOR_EACH_POPULATED_DIMM(dimms, idx) \
	FOR_EACH_DIMM(idx) IF_DIMM_POPULATED(dimms, idx)
#define FOR_EACH_DIMM_IN_CHANNEL(ch, idx) \
	for (idx = (ch) << 1; idx < ((ch) << 1) + DIMMS_PER_CHANNEL; ++idx)
#define FOR_EACH_POPULATED_DIMM_IN_CHANNEL(dimms, ch, idx) \
	FOR_EACH_DIMM_IN_CHANNEL(ch, idx) IF_DIMM_POPULATED(dimms, idx)
#define CHANNEL_IS_POPULATED(dimms, idx) \
	((dimms[idx<<1].card_type != RAW_CARD_UNPOPULATED) \
		|| (dimms[(idx<<1) + 1].card_type != RAW_CARD_UNPOPULATED))
#define CHANNEL_IS_CARDF(dimms, idx) \
	((dimms[idx<<1].card_type == 0xf) \
		|| (dimms[(idx<<1) + 1].card_type == 0xf))
#define IF_CHANNEL_POPULATED(dimms, idx) \
	if ((dimms[idx<<1].card_type != RAW_CARD_UNPOPULATED) \
		|| (dimms[(idx<<1) + 1].card_type != RAW_CARD_UNPOPULATED))
#define FOR_EACH_CHANNEL(idx) \
	for (idx = 0; idx < TOTAL_CHANNELS; ++idx)
#define FOR_EACH_POPULATED_CHANNEL(dimms, idx) \
	FOR_EACH_CHANNEL(idx) IF_CHANNEL_POPULATED(dimms, idx)

#define RANKS_PER_CHANNEL 4
#define RANK_IS_POPULATED(dimms, ch, r) \
	(((dimms[ch<<1].card_type != RAW_CARD_UNPOPULATED) && ((r) < dimms[ch<<1].ranks)) || \
	((dimms[(ch<<1) + 1].card_type != RAW_CARD_UNPOPULATED) && ((r) >= 2) && ((r) < (dimms[(ch<<1) + 1].ranks + 2))))
#define IF_RANK_POPULATED(dimms, ch, r) \
	if (((dimms[ch<<1].card_type != RAW_CARD_UNPOPULATED) \
	&& ((r) < dimms[ch<<1].ranks)) \
	|| ((dimms[(ch<<1) + 1].card_type != RAW_CARD_UNPOPULATED) \
		&& ((r) >= 2) && ((r) < (dimms[(ch<<1) + 1].ranks + 2))))
#define FOR_EACH_RANK_IN_CHANNEL(r) \
	for (r = 0; r < RANKS_PER_CHANNEL; ++r)
#define FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, r) \
	FOR_EACH_RANK_IN_CHANNEL(r) IF_RANK_POPULATED(dimms, ch, r)
#define FOR_EACH_RANK(ch, r) \
	FOR_EACH_CHANNEL(ch) FOR_EACH_RANK_IN_CHANNEL(r)
#define FOR_EACH_POPULATED_RANK(dimms, ch, r) \
	FOR_EACH_RANK(ch, r) IF_RANK_POPULATED(dimms, ch, r)
#define FOR_EACH_BYTELANE(l) \
	for (l = 0; l < TOTAL_BYTELANES; l++)
#define FOR_EACH_POPULATED_CHANNEL_AND_BYTELANE(dimms, ch, l) \
	FOR_EACH_POPULATED_CHANNEL (dimms, ch) FOR_EACH_BYTELANE(l)

#define DDR3_MAX_CAS 18

enum fsb_clock {
	FSB_CLOCK_800MHz	= 0,
	FSB_CLOCK_1066MHz	= 1,
	FSB_CLOCK_1333MHz	= 2,
};

enum mem_clock {
	MEM_CLOCK_400MHz  = 0,
	MEM_CLOCK_533MHz  = 1,
	MEM_CLOCK_667MHz  = 2,
	MEM_CLOCK_800MHz  = 3,
	MEM_CLOCK_1066MHz  = 4,
	MEM_CLOCK_1333MHz  = 5,
};

enum ddr {
	DDR2 = 2,
	DDR3 = 3,
};

enum ddrxspd {
	DDR2SPD = 0x8,
	DDR3SPD = 0xb,
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

struct dll_setting {
	u8 tap;
	u8 pi;
	u8 db_en;
	u8 db_sel;
	u8 clk_delay;
	u8 coarse;
};

struct rt_dqs_setting {
	u8 tap;
	u8 pi;
};

enum n_banks {
	N_BANKS_4 = 0,
	N_BANKS_8 = 1,
};

struct timings {
	unsigned int	CAS;
	unsigned int    tclk;
	enum fsb_clock	fsb_clk;
	enum mem_clock	mem_clk;
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
	unsigned int	card_type; /* 0xff: unpopulated,
				      0xa - 0xf: raw card type A - F */
	enum chip_width	width;
	unsigned int	page_size; /* of whole DIMM in Bytes (4096 or 8192) */
	enum n_banks	n_banks;
	unsigned int	ranks;
	unsigned int	rows;
	unsigned int	cols;
	u16             spd_crc;
};

struct rcven_timings {
	u8 min_common_coarse;
	u8 coarse_offset[TOTAL_BYTELANES];
	u8 medium[TOTAL_BYTELANES];
	u8 tap[TOTAL_BYTELANES];
	u8 pi[TOTAL_BYTELANES];
};

/* The setup is up to two DIMMs per channel */
struct sysinfo {
	int		boot_path;
	int		max_ddr2_mhz;
	enum fsb_clock	max_fsb;

	int		dimm_config[2];
	int		spd_type;
	int		channel_capacity[2];
	struct timings	selected_timings;
	struct dimminfo	dimms[4];
	u8		spd_map[4];
	struct rcven_timings rcven_t[TOTAL_CHANNELS];
	/*
	 * The rt_dqs delay register for rank 0 seems to be used
	 * for all other ranks on the channel, so only save that
	 */
	struct rt_dqs_setting rt_dqs[TOTAL_CHANNELS][TOTAL_BYTELANES];
	struct dll_setting dqs_settings[TOTAL_CHANNELS][TOTAL_BYTELANES];
	struct dll_setting dq_settings[TOTAL_CHANNELS][TOTAL_BYTELANES];
};
#define BOOT_PATH_NORMAL	0
#define BOOT_PATH_WARM_RESET	1
#define BOOT_PATH_RESUME	2

enum ddr2_signals {
	CLKSET0 = 0,
	CTRL0,
	CLKSET1,
	CMD,
	CTRL1,
	CTRL2,
	CTRL3,
};

#ifndef __BOOTBLOCK__
void x4x_early_init(void);
void x4x_late_init(int s3resume);
u32 decode_igd_memory_size(u32 gms);
u32 decode_igd_gtt_size(u32 gsm);
u8 decode_pciebar(u32 *const base, u32 *const len);
void sdram_initialize(int boot_path, const u8 *spd_map);
void raminit_ddr2(struct sysinfo *s, int fast_boot);
void rcven(struct sysinfo *s);
u32 fsb2mhz(u32 speed);
u32 ddr2mhz(u32 speed);
u32 test_address(int channel, int rank);

extern const struct dll_setting default_ddr2_667_ctrl[7];
extern const struct dll_setting default_ddr2_800_ctrl[7];
extern const struct dll_setting default_ddr3_800_ctrl[2][7];
extern const struct dll_setting default_ddr3_1067_ctrl[2][7];
extern const struct dll_setting default_ddr3_1333_ctrl[2][7];
extern const struct dll_setting default_ddr2_667_dqs[TOTAL_BYTELANES];
extern const struct dll_setting default_ddr2_800_dqs[TOTAL_BYTELANES];
extern const struct dll_setting default_ddr3_800_dqs[2][TOTAL_BYTELANES];
extern const struct dll_setting default_ddr3_1067_dqs[2][TOTAL_BYTELANES];
extern const struct dll_setting default_ddr3_1333_dqs[2][TOTAL_BYTELANES];
extern const struct dll_setting default_ddr2_667_dq[TOTAL_BYTELANES];
extern const struct dll_setting default_ddr2_800_dq[TOTAL_BYTELANES];
extern const struct dll_setting default_ddr3_800_dq[2][TOTAL_BYTELANES];
extern const struct dll_setting default_ddr3_1067_dq[2][TOTAL_BYTELANES];
extern const struct dll_setting default_ddr3_1333_dq[2][TOTAL_BYTELANES];

struct acpi_rsdp;
#ifndef __SIMPLE_DEVICE__
unsigned long northbridge_write_acpi_tables(device_t device, unsigned long start, struct acpi_rsdp *rsdp);
#endif /* __SIMPLE_DEVICE__ */
#endif
#endif /* __NORTHBRIDGE_INTEL_X4X_H__ */
