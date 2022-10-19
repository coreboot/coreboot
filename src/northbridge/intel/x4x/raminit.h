/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __X4X_RAMINIT_H__
#define __X4X_RAMINIT_H__

#include <types.h>

#define NOP_CMD		0x2
#define PRECHARGE_CMD	0x4
#define MRS_CMD		0x6
#define EMRS_CMD	0x8
#define EMRS1_CMD	(EMRS_CMD | 0x10)
#define EMRS2_CMD	(EMRS_CMD | 0x20)
#define EMRS3_CMD	(EMRS_CMD | 0x30)
#define ZQCAL_CMD	0xa
#define CBR_CMD		0xc
#define NORMALOP_CMD	0xe

#define TOTAL_CHANNELS		2
#define TOTAL_DIMMS		4
#define TOTAL_BYTELANES		8
#define DIMMS_PER_CHANNEL	(TOTAL_DIMMS / TOTAL_CHANNELS)
#define RAW_CARD_UNPOPULATED	0xff
#define RAW_CARD_POPULATED	0

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
	FSB_CLOCK_800MHz  = 0,
	FSB_CLOCK_1066MHz = 1,
	FSB_CLOCK_1333MHz = 2,
};

enum mem_clock {
	MEM_CLOCK_400MHz  = 0,
	MEM_CLOCK_533MHz  = 1,
	MEM_CLOCK_667MHz  = 2,
	MEM_CLOCK_800MHz  = 3,
	MEM_CLOCK_1066MHz = 4,
	MEM_CLOCK_1333MHz = 5,
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
	unsigned int	tclk;
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
	unsigned int	card_type; /* 0xff: unpopulated, 0xa - 0xf: raw card type A - F */
	enum chip_width	width;
	unsigned int	page_size; /* of whole DIMM in Bytes (4096 or 8192) */
	enum n_banks	n_banks;
	unsigned int	ranks;
	unsigned int	rows;
	unsigned int	cols;
	u16		spd_crc;
	u8		mirrored;
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
	u8		nmode;
	u8		stacked_mode;
};

enum ddr2_signals {
	CLKSET0 = 0,
	CTRL0,
	CLKSET1,
	CMD,
	CTRL1,
	CTRL2,
	CTRL3,
};

void sdram_initialize(int boot_path, const u8 *spd_map);
void do_raminit(struct sysinfo *, int fast_boot);
void rcven(struct sysinfo *s);
u32 fsb_to_mhz(u32 speed);
u32 ddr_to_mhz(u32 speed);
u32 test_address(int channel, int rank);
void dqsset(u8 ch, u8 lane, const struct dll_setting *setting);
void dqset(u8 ch, u8 lane, const struct dll_setting *setting);
void rt_set_dqs(u8 channel, u8 lane, u8 rank, struct rt_dqs_setting *dqs_setting);
enum cb_err do_write_training(struct sysinfo *s);
enum cb_err do_read_training(struct sysinfo *s);
void search_write_leveling(struct sysinfo *s);
void send_jedec_cmd(const struct sysinfo *s, u8 r, u8 ch, u8 cmd, u32 val);

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
extern const u8 ddr3_emrs1_rtt_nom_config[16][4];
extern const u8 post_jedec_tab[3][4][2];
extern const u32 ddr3_c2_tab[2][3][6][2];
extern const u8 ddr3_c2_x264[3][6];
extern const u16 ddr3_c2_x23c[3][6];

#endif /* __X4X_RAMINIT_H__ */
