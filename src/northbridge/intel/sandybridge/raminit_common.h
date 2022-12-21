/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_COMMON_H
#define RAMINIT_COMMON_H

#include <device/dram/ddr3.h>
#include <stdint.h>

#define BASEFREQ	133
#define tDLLK		512

#define NUM_CHANNELS	2
#define NUM_SLOTRANKS	4
#define NUM_SLOTS	2
#define NUM_LANES	9

/* IOSAV_n_SP_CMD_CTRL DRAM commands */
#define IOSAV_MRS		(0xf000)
#define IOSAV_PRE		(0xf002)
#define IOSAV_ZQCS		(0xf003)
#define IOSAV_ACT		(0xf006)
#define IOSAV_RD		(0xf105)
#define IOSAV_NOP_ALT		(0xf107)
#define IOSAV_WR		(0xf201)
#define IOSAV_NOP		(0xf207)

/* IOSAV_n_SUBSEQ_CTRL data direction */
#define SSQ_NA			0 /* Non-data */
#define SSQ_RD			1 /* Read */
#define SSQ_WR			2 /* Write */
#define SSQ_RW			3 /* Read and write */

struct iosav_ssq {
	/* IOSAV_n_SP_CMD_CTRL */
	union {
		struct {
			u32 command    : 16; /* [15.. 0] */
			u32 ranksel_ap :  2; /* [17..16] */
			u32            : 14;
		};
		u32 raw;
	} sp_cmd_ctrl;

	/* IOSAV_n_SUBSEQ_CTRL */
	union {
		struct {
			u32 cmd_executions : 9; /* [ 8.. 0] */
			u32                : 1;
			u32 cmd_delay_gap  : 5; /* [14..10] */
			u32                : 1;
			u32 post_ssq_wait  : 9; /* [24..16] */
			u32                : 1;
			u32 data_direction : 2; /* [27..26] */
			u32                : 4;
		};
		u32 raw;
	} subseq_ctrl;

	/* IOSAV_n_SP_CMD_ADDR */
	union {
		struct {
			u32 address : 16; /* [15.. 0] */
			u32 rowbits :  3; /* [18..16] */
			u32         :  1;
			u32 bank    :  3; /* [22..20] */
			u32         :  1;
			u32 rank    :  2; /* [25..24] */
			u32         :  6;
		};
		u32 raw;
	} sp_cmd_addr;

	/* IOSAV_n_ADDR_UPDATE */
	union {
		struct {
			u32 inc_addr_1 :  1; /* [ 0.. 0] */
			u32 inc_addr_8 :  1; /* [ 1.. 1] */
			u32 inc_bank   :  1; /* [ 2.. 2] */
			u32 inc_rank   :  2; /* [ 4.. 3] */
			u32 addr_wrap  :  5; /* [ 9.. 5] */
			u32 lfsr_upd   :  2; /* [11..10] */
			u32 upd_rate   :  4; /* [15..12] */
			u32 lfsr_xors  :  2; /* [17..16] */
			u32            : 14;
		};
		u32 raw;
	} addr_update;
};

union gdcr_rx_reg {
	struct {
		u32 rcven_pi_code     : 6; /* [ 5.. 0] */
		u32                   : 2;
		u32 rx_dqs_p_pi_code  : 7; /* [14.. 8] */
		u32                   : 1;
		u32 rcven_logic_delay : 3; /* [18..16] */
		u32                   : 1;
		u32 rx_dqs_n_pi_code  : 7; /* [26..20] */
		u32                   : 5;
	};
	u32 raw;
};

union gdcr_tx_reg {
	struct {
		u32 tx_dq_pi_code      :  6; /* [ 5.. 0] */
		u32                    :  2;
		u32 tx_dqs_pi_code     :  6; /* [13.. 8] */
		u32                    :  1;
		u32 tx_dqs_logic_delay :  3; /* [17..15] */
		u32                    :  1;
		u32 tx_dq_logic_delay  :  1; /* [19..19] */
		u32                    : 12;
	};
	u32 raw;
};

union gdcr_cmd_pi_coding_reg {
	struct {
		u32 cmd_pi_code         : 6; /* [ 5.. 0] */
		u32 ctl_pi_code_d0      : 6; /* [11.. 6] */
		u32 cmd_logic_delay     : 1; /* [12..12] */
		u32 cmd_phase_delay     : 1; /* [13..13] */
		u32 cmd_xover_enable    : 1; /* [14..14] */
		u32 ctl_logic_delay_d0  : 1; /* [15..15] */
		u32 ctl_phase_delay_d0  : 1; /* [16..16] */
		u32 ctl_xover_enable_d0 : 1; /* [17..17] */
		u32 ctl_pi_code_d1      : 6; /* [23..18] */
		u32 ctl_logic_delay_d1  : 1; /* [24..24] */
		u32 ctl_phase_delay_d1  : 1; /* [25..25] */
		u32 ctl_xover_enable_d1 : 1; /* [26..26] */
		u32                     : 5;
	};
	u32 raw;
};

union gdcr_training_mod_reg {
	struct {
		u32 receive_enable_mode : 1; /* [ 0.. 0] */
		u32 write_leveling_mode : 1; /* [ 1.. 1] */
		u32 training_rank_sel   : 2; /* [ 3.. 2] */
		u32 enable_dqs_wl       : 4; /* [ 7.. 4] */
		u32 dqs_logic_delay_wl  : 1; /* [ 8.. 8] */
		u32 dq_dqs_training_res : 1; /* [ 9.. 9] */
		u32                     : 4;
		u32 delay_dq            : 1; /* [14..14] */
		u32 odt_always_on       : 1; /* [15..15] */
		u32                     : 4;
		u32 force_drive_enable  : 1; /* [20..20] */
		u32 dft_tx_pi_clk_view  : 1; /* [21..21] */
		u32 dft_tx_pi_clk_swap  : 1; /* [22..22] */
		u32 early_odt_en        : 1; /* [23..23] */
		u32 vref_gen_ctl        : 6; /* [29..24] */
		u32 ext_vref_sel        : 1; /* [30..30] */
		u32 tx_fifo_always_on   : 1; /* [31..31] */
	};
	u32 raw;
};

union comp_ofst_1_reg {
	struct {
		u32 dq_odt_down  : 3; /* [ 2.. 0] */
		u32 dq_odt_up    : 3; /* [ 5.. 3] */
		u32 clk_odt_down : 3; /* [ 8.. 6] */
		u32 clk_odt_up   : 3; /* [11.. 9] */
		u32 dq_drv_down  : 3; /* [14..12] */
		u32 dq_drv_up    : 3; /* [17..15] */
		u32 clk_drv_down : 3; /* [20..18] */
		u32 clk_drv_up   : 3; /* [23..21] */
		u32 ctl_drv_down : 3; /* [26..24] */
		u32 ctl_drv_up   : 3; /* [29..27] */
		u32              : 2;
	};
	u32 raw;
};

union tc_dbp_reg {
	struct {
		u32 tRCD : 4; /* [ 3.. 0] */
		u32 tRP  : 4; /* [ 7.. 4] */
		u32 tAA  : 4; /* [11.. 8] */
		u32 tCWL : 4; /* [15..12] */
		u32 tRAS : 8; /* [23..16] */
		u32      : 8;
	};
	u32 raw;
};

union tc_rap_reg {
	struct {
		u32 tRRD    : 4; /* [ 3.. 0] */
		u32 tRTP    : 4; /* [ 7.. 4] */
		u32 tCKE    : 4; /* [11.. 8] */
		u32 tWTR    : 4; /* [15..12] */
		u32 tFAW    : 8; /* [23..16] */
		u32 tWR     : 5; /* [28..24] */
		u32 dis_3st : 1; /* [29..29] */
		u32 tCMD    : 2; /* [31..30] */
	};
	u32 raw;
};

union tc_rwp_reg {
	struct {
		u32 tRRDR   : 3; /* [ 2.. 0] */
		u32         : 1;
		u32 tRRDD   : 3; /* [ 6.. 4] */
		u32         : 1;
		u32 tWWDR   : 3; /* [10.. 8] */
		u32         : 1;
		u32 tWWDD   : 3; /* [14..12] */
		u32         : 1;
		u32 tRWDRDD : 3; /* [18..16] */
		u32         : 1;
		u32 tWRDRDD : 3; /* [22..20] */
		u32         : 1;
		u32 tRWSR   : 3; /* [26..24] */
		u32 dec_wrd : 1; /* [27..27] */
		u32         : 4;
	};
	u32 raw;
};

union tc_othp_reg {
	struct {
		u32 tXPDLL       :  5; /* [ 4.. 0] */
		u32 tXP          :  3; /* [ 7.. 5] */
		u32 tAONPD       :  4; /* [11.. 8] */
		u32 tCPDED       :  2; /* [13..12] */
		u32 tPRPDEN      :  2; /* [15..14] */
		u32 odt_delay_d0 :  2; /* [17..16] */
		u32 odt_delay_d1 :  2; /* [19..18] */
		u32              : 12;
	};
	u32 raw;
};

union tc_dtp_reg {
	struct {
		u32                  : 12;
		u32 overclock_tXP    :  1; /* [12..12] */
		u32 overclock_tXPDLL :  1; /* [13..13] */
		u32                  : 18;
	};
	u32 raw;
};

union tc_rfp_reg {
	struct {
		u32 oref_ri            :  8; /* [ 7.. 0] */
		u32 refresh_high_wm    :  4; /* [11.. 8] */
		u32 refresh_panic_wm   :  4; /* [15..12] */
		u32 refresh_2x_control :  2; /* [17..16] */
		u32                    : 14;
	};
	u32 raw;
};

union tc_rftp_reg {
	struct {
		u32 tREFI   : 16; /* [15.. 0] */
		u32 tRFC    :  9; /* [24..16] */
		u32 tREFIx9 :  7; /* [31..25] */
	};
	u32 raw;
};

union tc_srftp_reg {
	struct {
		u32 tXSDLL     : 12; /* [11.. 0] */
		u32 tXS_offset :  4; /* [15..12] */
		u32 tZQOPER    : 10; /* [25..16] */
		u32            :  2;
		u32 tMOD       :  4; /* [31..28] */
	};
	u32 raw;
};

typedef struct ramctr_timing_st ramctr_timing;

void iosav_write_sequence(const int ch, const struct iosav_ssq *seq, const unsigned int length);
void iosav_run_queue(const int ch, const u8 loops, const u8 as_timer);
void wait_for_iosav(int channel);
void iosav_run_once_and_wait(const int ch);

void iosav_write_zqcs_sequence(int channel, int slotrank, u32 gap, u32 post, u32 wrap);
void iosav_write_prea_sequence(int channel, int slotrank, u32 post, u32 wrap);
void iosav_write_read_mpr_sequence(
	int channel, int slotrank, u32 tMOD, u32 loops, u32 gap, u32 loops2, u32 post2);
void iosav_write_prea_act_read_sequence(ramctr_timing *ctrl, int channel, int slotrank);
void iosav_write_jedec_write_leveling_sequence(
	ramctr_timing *ctrl, int channel, int slotrank, int bank, u32 mr1reg);
void iosav_write_misc_write_sequence(ramctr_timing *ctrl, int channel, int slotrank,
				     u32 gap0, u32 loops0, u32 gap1, u32 loops2, u32 wrap2);
void iosav_write_command_training_sequence(
	ramctr_timing *ctrl, int channel, int slotrank, unsigned int address);
void iosav_write_data_write_sequence(ramctr_timing *ctrl, int channel, int slotrank);
void iosav_write_aggressive_write_read_sequence(ramctr_timing *ctrl, int channel, int slotrank);
void iosav_write_memory_test_sequence(ramctr_timing *ctrl, int channel, int slotrank);

/* FIXME: Vendor BIOS uses 64 but our algorithms are less
   performant and even 1 seems to be enough in practice.  */
#define NUM_PATTERNS	4

/*
 * WARNING: Do not forget to increase MRC_CACHE_VERSION when the saved data is changed!
 */
#define MRC_CACHE_VERSION 5

enum power_down_mode {
	PDM_NONE        = 0,
	PDM_APD         = 1,
	PDM_PPD         = 2,
	PDM_APD_PPD     = 3,
	PDM_DLL_OFF     = 6,
	PDM_APD_DLL_OFF = 7,
};

typedef struct odtmap_st {
	u16 rttwr;
	u16 rttnom;
} odtmap;

/* WARNING: Do not forget to increase MRC_CACHE_VERSION when this struct is changed! */
typedef struct dimm_info_st {
	struct dimm_attr_ddr3_st dimm[NUM_CHANNELS][NUM_SLOTS];
} dimm_info;

/* WARNING: Do not forget to increase MRC_CACHE_VERSION when this struct is changed! */
struct ram_rank_timings {
	/* ROUNDT_LAT register: One byte per slotrank */
	u8 roundtrip_latency;

	/* IO_LATENCY register: One nibble per slotrank */
	u8 io_latency;

	/* Phase interpolator coding for command and control */
	int pi_coding;

	struct ram_lane_timings {
		/* GDCR RX timings */
		u16 rcven;
		u8 rx_dqs_p;
		u8 rx_dqs_n;

		/* GDCR TX timings */
		int tx_dq;
		u16 tx_dqs;
	} lanes[NUM_LANES];
};

/* WARNING: Do not forget to increase MRC_CACHE_VERSION when this struct is changed! */
typedef struct ramctr_timing_st {
	u16 spd_crc[NUM_CHANNELS][NUM_SLOTS];

	/* CPUID value */
	u32 cpu;

	/* DDR base_freq = 100 Mhz / 133 Mhz */
	u8 base_freq;

	/* Frequency index */
	u32 FRQ;

	u16 cas_supported;
	/* Latencies are in units of ns, scaled by x256 */
	u32 tCK;
	u32 tAA;
	u32 tWR;
	u32 tRCD;
	u32 tRRD;
	u32 tRP;
	u32 tRAS;
	u32 tRFC;
	u32 tWTR;
	u32 tRTP;
	u32 tFAW;
	u32 tCWL;
	u32 tCMD;
	/* Latencies in terms of clock cycles
	   They are saved separately as they are needed for DRAM MRS commands */
	u8 CAS;			/* CAS read  latency */
	u8 CWL;			/* CAS write latency */

	u32 tREFI;
	u32 tMOD;
	u32 tXSOffset;
	u32 tWLO;
	u32 tCKE;
	u32 tXPDLL;
	u32 tXP;
	u32 tAONPD;

	/* Bits [0..11] of PM_DLL_CONFIG: Master DLL wakeup delay timer */
	u16 mdll_wake_delay;

	u8 rankmap[NUM_CHANNELS];
	int ref_card_offset[NUM_CHANNELS];
	u32 mad_dimm[NUM_CHANNELS];
	int channel_size_mb[NUM_CHANNELS];
	u32 cmd_stretch[NUM_CHANNELS];

	int pi_code_offset;
	int pi_coding_threshold;

	bool ecc_supported;
	bool ecc_forced;
	bool ecc_enabled;
	int lanes;	/* active lanes: 8 or 9 */
	int edge_offset[3];
	int tx_dq_offset[3];

	int extended_temperature_range;
	int auto_self_refresh;

	int rank_mirror[NUM_CHANNELS][NUM_SLOTRANKS];

	struct ram_rank_timings timings[NUM_CHANNELS][NUM_SLOTRANKS];

	dimm_info info;
} ramctr_timing;

#define SOUTHBRIDGE	PCI_DEV(0, 0x1f, 0)

#define FOR_ALL_LANES for (lane = 0; lane < ctrl->lanes; lane++)
#define FOR_ALL_CHANNELS for (channel = 0; channel < NUM_CHANNELS; channel++)
#define FOR_ALL_POPULATED_RANKS for (slotrank = 0; slotrank < NUM_SLOTRANKS; slotrank++) if (ctrl->rankmap[channel] & (1 << slotrank))
#define FOR_ALL_POPULATED_CHANNELS for (channel = 0; channel < NUM_CHANNELS; channel++) if (ctrl->rankmap[channel])
#define MAX_EDGE_TIMING 71
#define MAX_TX_DQ 127
#define MAX_TX_DQS 511
#define MAX_RCVEN 127
#define MAX_CAS 18
#define MIN_CAS 4

/*
 * 1 QCLK (quadrature clock) is one half of a full clock cycle (tCK).
 * In addition, 64 PI (phase interpolator) ticks are equal to 1 QCLK.
 * Logic delay values in I/O register bitfields are expressed in QCLKs.
 */
#define QCLK_PI	64

#define MAKE_ERR		((channel << 16) | (slotrank << 8) | 1)
#define GET_ERR_CHANNEL(x)	(x >> 16)

void dram_mrscommands(ramctr_timing *ctrl);
void program_timings(ramctr_timing *ctrl, int channel);
void dram_find_common_params(ramctr_timing *ctrl);
void dram_xover(ramctr_timing *ctrl);
void dram_timing_regs(ramctr_timing *ctrl);
void dram_dimm_mapping(ramctr_timing *ctrl);
void dram_dimm_set_mapping(ramctr_timing *ctrl, int training);
void dram_zones(ramctr_timing *ctrl, int training);
void dram_memorymap(ramctr_timing *ctrl, int me_uma_size);
void dram_jedecreset(ramctr_timing *ctrl);
int receive_enable_calibration(ramctr_timing *ctrl);
int write_training(ramctr_timing *ctrl);
int command_training(ramctr_timing *ctrl);
int read_mpr_training(ramctr_timing *ctrl);
int aggressive_read_training(ramctr_timing *ctrl);
int aggressive_write_training(ramctr_timing *ctrl);
void normalize_training(ramctr_timing *ctrl);
int channel_test(ramctr_timing *ctrl);
void set_scrambling_seed(ramctr_timing *ctrl);
void set_wmm_behavior(const u32 cpu);
void prepare_training(ramctr_timing *ctrl);
void set_read_write_timings(ramctr_timing *ctrl);
void set_normal_operation(ramctr_timing *ctrl);
void final_registers(ramctr_timing *ctrl);
void restore_timings(ramctr_timing *ctrl);
int try_init_dram_ddr3(ramctr_timing *ctrl, int fast_boot, int s3resume, int me_uma_size);

void channel_scrub(ramctr_timing *ctrl);
bool get_host_ecc_cap(void);
bool get_host_ecc_forced(void);

#endif
