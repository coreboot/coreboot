/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_COMMON_H
#define RAMINIT_COMMON_H

#include <stdint.h>

#define BASEFREQ	133
#define tDLLK		512

#define IS_SANDY_CPU(x)    ((x & 0xffff0) == 0x206a0)
#define IS_SANDY_CPU_C(x)  ((x & 0xf) == 4)
#define IS_SANDY_CPU_D0(x) ((x & 0xf) == 5)
#define IS_SANDY_CPU_D1(x) ((x & 0xf) == 6)
#define IS_SANDY_CPU_D2(x) ((x & 0xf) == 7)

#define IS_IVY_CPU(x)   ((x & 0xffff0) == 0x306a0)
#define IS_IVY_CPU_C(x) ((x & 0xf) == 4)
#define IS_IVY_CPU_K(x) ((x & 0xf) == 5)
#define IS_IVY_CPU_D(x) ((x & 0xf) == 6)
#define IS_IVY_CPU_E(x) ((x & 0xf) >= 8)

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

#define ZQCS_SEQUENCE(slotrank, gap, post, wrap)					\
	{										\
		/* DRAM command ZQCS */							\
		[0] = {									\
			.sp_cmd_ctrl = {						\
				.command = IOSAV_ZQCS,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = gap,					\
				.post_ssq_wait  = post,					\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.addr_wrap = wrap,					\
			},								\
		},									\
	}

#define PREA_SEQUENCE(t_rp, wrap)							\
	{										\
		/* DRAM command PREA */							\
		[0] = {									\
			.sp_cmd_ctrl = {						\
				.command = IOSAV_PRE,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = 3,					\
				.post_ssq_wait  = t_rp,					\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 1024,					\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.addr_wrap = wrap,					\
			},								\
		},									\
	}

#define READ_MPR_SEQUENCE(t_mod, loops, gap, loops2, post2)				\
	{										\
		/*									\
		 * DRAM command MRS							\
		 *									\
		 * Write MR3 MPR enable. In this mode only RD and RDA			\
		 * are allowed, and all reads return a predefined pattern.		\
		 */									\
		[0] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_MRS,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = 3,					\
				.post_ssq_wait  = t_mod,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 4,						\
				.rowbits = 6,						\
				.bank    = 3,						\
				.rank    = slotrank,					\
			},								\
		},									\
		/* DRAM command RD */							\
		[1] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_RD,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = loops,				\
				.cmd_delay_gap  = gap,					\
				.post_ssq_wait  = 4,					\
				.data_direction = SSQ_RD,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
		},									\
		/* DRAM command RD */							\
		[2] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_RD,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = loops2,				\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = post2,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
		},									\
		/*									\
		 * DRAM command MRS							\
		 *									\
		 * Write MR3 MPR disable.						\
		 */									\
		[3] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_MRS,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = 3,					\
				.post_ssq_wait  = t_mod,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 6,						\
				.bank    = 3,						\
				.rank    = slotrank,					\
			},								\
		},									\
	}

#define MISC_WRITE_SEQUENCE(gap0, loops0, gap1, loops2, wrap2)				\
	{										\
		/* DRAM command ACT */							\
		[0] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_ACT,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = loops0,				\
				.cmd_delay_gap  = gap0,					\
				.post_ssq_wait  = ctrl->tRCD,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_bank  = loops0 == 1 ? 0 : 1,			\
				.addr_wrap = loops0 == 1 ? 0 : 18,			\
			},								\
		},									\
		/* DRAM command NOP */							\
		[1] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_NOP,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = gap1,					\
				.post_ssq_wait  = 4,					\
				.data_direction = SSQ_WR,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 8,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.addr_wrap = 31,					\
			},								\
		},									\
		/* DRAM command WR */							\
		[2] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_WR,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = loops2,				\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = 4,					\
				.data_direction = SSQ_WR,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = wrap2,					\
			},								\
		},									\
		/* DRAM command NOP */							\
		[3] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_NOP,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = 3,					\
				.post_ssq_wait  = ctrl->CWL + ctrl->tWTR + 5,		\
				.data_direction = SSQ_WR,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 8,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.addr_wrap = 31,					\
			},								\
		},									\
	}

#define COMMAND_TRAINING_SEQUENCE(ctr)							\
	{										\
		/* DRAM command ACT */							\
		[0] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_ACT,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 8,					\
				.cmd_delay_gap  = MAX(ctrl->tRRD, (ctrl->tFAW >> 2) + 1), \
				.post_ssq_wait  = ctrl->tRCD,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = ctr,						\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_bank  = 1,						\
				.addr_wrap = 18,					\
			},								\
		},									\
		/* DRAM command WR */							\
		[1] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_WR,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 32,					\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = ctrl->CWL + ctrl->tWTR + 8,		\
				.data_direction = SSQ_WR,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = 18,					\
				.lfsr_upd   = 3,					\
				.lfsr_xors  = 2,					\
			},								\
		},									\
		/* DRAM command RD */							\
		[2] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_RD,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 32,					\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = MAX(ctrl->tRTP, 8),			\
				.data_direction = SSQ_RD,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = 18,					\
				.lfsr_upd   = 3,					\
				.lfsr_xors  = 2,					\
			},								\
		},									\
		/* DRAM command PRE */							\
		[3] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_PRE,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = 15,					\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 1024,					\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.addr_wrap  = 18,					\
			},								\
		},									\
	}

#define WRITE_DATA_SEQUENCE								\
	{										\
		/* DRAM command ACT */							\
		[0] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_ACT,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 4,					\
				.cmd_delay_gap = MAX(ctrl->tRRD, (ctrl->tFAW >> 2) + 1), \
				.post_ssq_wait  = ctrl->tRCD,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_bank  = 0,						\
				.addr_wrap = 18,					\
			},								\
		},									\
		/* DRAM command WR */							\
		[1] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_WR,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 32,					\
				.cmd_delay_gap  = 20,					\
				.post_ssq_wait  = ctrl->CWL + ctrl->tWTR + 8,		\
				.data_direction = SSQ_WR,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = 18,					\
			},								\
		},									\
		/* DRAM command RD */							\
		[2] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_RD,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 32,					\
				.cmd_delay_gap  = 20,					\
				.post_ssq_wait  = MAX(ctrl->tRTP, 8),			\
				.data_direction = SSQ_RD,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = 18,					\
			},								\
		},									\
		/* DRAM command PRE */							\
		[3] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_PRE,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = 3,					\
				.post_ssq_wait  = ctrl->tRP,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 1024,					\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
		},									\
	}

#define AGGRESSIVE_WRITE_READ_SEQUENCE							\
	{										\
		/* DRAM command ACT */							\
		[0] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_ACT,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 4,					\
				.cmd_delay_gap = MAX((ctrl->tFAW >> 2) + 1, ctrl->tRRD), \
				.post_ssq_wait  = ctrl->tRCD,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_bank  = 1,						\
				.addr_wrap = 18,					\
			},								\
		},									\
		/* DRAM command WR */							\
		[1] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_WR,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 480,					\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = ctrl->tWTR + ctrl->CWL + 8,		\
				.data_direction = SSQ_WR,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = 18,					\
			},								\
		},									\
		/* DRAM command RD */							\
		[2] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_RD,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 480,					\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = MAX(ctrl->tRTP, 8),			\
				.data_direction = SSQ_RD,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = 18,					\
			},								\
		},									\
		/* DRAM command PRE */							\
		[3] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_PRE,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = ctrl->tRP,				\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 1024,					\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
		},									\
	}

#define MEMORY_TEST_SEQUENCE								\
	{										\
		/* DRAM command ACT */							\
		[0] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_ACT,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 4,					\
				.cmd_delay_gap  = 8,					\
				.post_ssq_wait  = 40,					\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_bank  = 1,						\
				.addr_wrap = 18,					\
			},								\
		},									\
		/* DRAM command WR */							\
		[1] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_WR,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 100,					\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = 40,					\
				.data_direction = SSQ_WR,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = 18,					\
			},								\
		},									\
		/* DRAM command RD */							\
		[2] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_RD,					\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 100,					\
				.cmd_delay_gap  = 4,					\
				.post_ssq_wait  = 40,					\
				.data_direction = SSQ_RD,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 0,						\
				.rowbits = 0,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.inc_addr_8 = 1,					\
				.addr_wrap  = 18,					\
			},								\
		},									\
		/* DRAM command PRE */							\
		[3] = {									\
			.sp_cmd_ctrl = {						\
				.command    = IOSAV_PRE,				\
				.ranksel_ap = 1,					\
			},								\
			.subseq_ctrl = {						\
				.cmd_executions = 1,					\
				.cmd_delay_gap  = 3,					\
				.post_ssq_wait  = 40,					\
				.data_direction = SSQ_NA,				\
			},								\
			.sp_cmd_addr = {						\
				.address = 1024,					\
				.rowbits = 6,						\
				.bank    = 0,						\
				.rank    = slotrank,					\
			},								\
			.addr_update = {						\
				.addr_wrap  = 18,					\
			},								\
		},									\
	}

void iosav_write_sequence(const int ch, const struct iosav_ssq *seq, const unsigned int length);
void iosav_run_queue(const int ch, const u8 loops, const u8 as_timer);
void iosav_run_once(const int ch);
void wait_for_iosav(int channel);

/* FIXME: Vendor BIOS uses 64 but our algorithms are less
   performant and even 1 seems to be enough in practice.  */
#define NUM_PATTERNS	4

/*
 * WARNING: Do not forget to increase MRC_CACHE_VERSION when the saved data is changed!
 */
#define MRC_CACHE_VERSION 5

typedef struct odtmap_st {
	u16 rttwr;
	u16 rttnom;
} odtmap;

/* WARNING: Do not forget to increase MRC_CACHE_VERSION when this struct is changed! */
typedef struct dimm_info_st {
	dimm_attr dimm[NUM_CHANNELS][NUM_SLOTS];
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
		/* Lane register offset 0x10 */
		u16 timA;	/* bits  0 -  5, bits 16 - 18 */
		u8 rising;	/* bits  8 - 14 */
		u8 falling;	/* bits 20 - 26 */

		/* Lane register offset 0x20 */
		int timC;	/* bits 0 -  5, 19 */
		u16 timB;	/* bits 8 - 13, 15 - 17 */
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
	int timC_offset[3];

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
#define MAX_TIMC 127
#define MAX_TIMB 511
#define MAX_TIMA 127
#define MAX_CAS 18
#define MIN_CAS 4

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
int read_training(ramctr_timing *ctrl);
int write_training(ramctr_timing *ctrl);
int command_training(ramctr_timing *ctrl);
int discover_edges(ramctr_timing *ctrl);
int discover_edges_write(ramctr_timing *ctrl);
int discover_timC_write(ramctr_timing *ctrl);
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
