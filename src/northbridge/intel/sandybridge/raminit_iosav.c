/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <types.h>

#include "raminit_native.h"
#include "raminit_common.h"
#include "raminit_tables.h"
#include "sandybridge.h"

/* FIXME: no support for 3-channel chipsets */

/* Number of programmed IOSAV subsequences. */
static unsigned int ssq_count = 0;

void iosav_write_sequence(const int ch, const struct iosav_ssq *seq, const unsigned int length)
{
	for (unsigned int i = 0; i < length; i++) {
		MCHBAR32(IOSAV_n_SP_CMD_CTRL_ch(ch, i)) = seq[i].sp_cmd_ctrl.raw;
		MCHBAR32(IOSAV_n_SUBSEQ_CTRL_ch(ch, i)) = seq[i].subseq_ctrl.raw;
		MCHBAR32(IOSAV_n_SP_CMD_ADDR_ch(ch, i)) = seq[i].sp_cmd_addr.raw;
		MCHBAR32(IOSAV_n_ADDR_UPDATE_ch(ch, i)) = seq[i].addr_update.raw;
	}

	ssq_count = length;
}

void iosav_run_queue(const int ch, const u8 loops, const u8 as_timer)
{
	/* Should never happen */
	if (ssq_count == 0)
		return;

	MCHBAR32(IOSAV_SEQ_CTL_ch(ch)) = loops | ((ssq_count - 1) << 18) | (as_timer << 22);
}

void wait_for_iosav(int channel)
{
	while (1) {
		if (MCHBAR32(IOSAV_STATUS_ch(channel)) & 0x50)
			return;
	}
}

void iosav_run_once_and_wait(const int ch)
{
	iosav_run_queue(ch, 1, 0);
	wait_for_iosav(ch);
}

void iosav_write_zqcs_sequence(int channel, int slotrank, u32 gap, u32 post, u32 wrap)
{
	const struct iosav_ssq sequence[] = {
		/* DRAM command ZQCS */
		[0] = {
			.sp_cmd_ctrl = {
				.command = IOSAV_ZQCS,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = gap,
				.post_ssq_wait  = post,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap = wrap,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_prea_sequence(int channel, int slotrank, u32 post, u32 wrap)
{
	const struct iosav_ssq sequence[] = {
		/* DRAM command PREA */
		[0] = {
			.sp_cmd_ctrl = {
				.command = IOSAV_PRE,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = post,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 1 << 10,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap = wrap,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_read_mpr_sequence(
	int channel, int slotrank, u32 tMOD, u32 loops, u32 gap, u32 loops2, u32 post2)
{
	const struct iosav_ssq sequence[] = {
		/*
		 * DRAM command MRS
		 *
		 * Write MR3 MPR enable. In this mode only RD and RDA
		 * are allowed, and all reads return a predefined pattern.
		 */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_MRS,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = tMOD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 4,
				.rowbits = 6,
				.bank    = 3,
				.rank    = slotrank,
			},
		},
		/* DRAM command RD */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_RD,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = loops,
				.cmd_delay_gap  = gap,
				.post_ssq_wait  = 4,
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
		},
		/* DRAM command RD */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_RD,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = loops2,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = post2,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
		},
		/*
		 * DRAM command MRS
		 *
		 * Write MR3 MPR disable.
		 */
		[3] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_MRS,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = tMOD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 3,
				.rank    = slotrank,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_prea_act_read_sequence(ramctr_timing *ctrl, int channel, int slotrank)
{
	const struct iosav_ssq sequence[] = {
		/* DRAM command PREA */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_PRE,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->tRP,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 1 << 10,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap = 18,
			},
		},
		/* DRAM command ACT */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_ACT,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 8,
				.cmd_delay_gap  = MAX(ctrl->tRRD, (ctrl->tFAW >> 2) + 1),
				.post_ssq_wait  = ctrl->CAS,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_bank  = 1,
				.addr_wrap = 18,
			},
		},
		/* DRAM command RD */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_RD,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 500,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = MAX(ctrl->tRTP, 8),
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
			},
		},
		/* DRAM command PREA */
		[3] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_PRE,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->tRP,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 1 << 10,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap = 18,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_jedec_write_leveling_sequence(
	ramctr_timing *ctrl, int channel, int slotrank, int bank, u32 mr1reg)
{
	/* First DQS/DQS# rising edge after write leveling mode is programmed */
	const u32 tWLMRD = 40;

	const struct iosav_ssq sequence[] = {
		/* DRAM command MRS: enable DQs on this slotrank */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_MRS,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = tWLMRD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = mr1reg,
				.rowbits = 6,
				.bank    = bank,
				.rank    = slotrank,
			},
		},
		/* DRAM command NOP */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_NOP,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->CWL + ctrl->tWLO,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 8,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
		},
		/* DRAM command NOP */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_NOP_ALT,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->CAS + 38,
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 4,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
		},
		/* DRAM command MRS: disable DQs on this slotrank */
		[3] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_MRS,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->tMOD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = mr1reg | 1 << 12,
				.rowbits = 6,
				.bank    = bank,
				.rank    = slotrank,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_misc_write_sequence(ramctr_timing *ctrl, int channel, int slotrank,
				     u32 gap0, u32 loops0, u32 gap1, u32 loops2, u32 wrap2)
{
	const struct iosav_ssq sequence[] = {
		/* DRAM command ACT */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_ACT,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = loops0,
				.cmd_delay_gap  = gap0,
				.post_ssq_wait  = ctrl->tRCD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_bank  = loops0 == 1 ? 0 : 1,
				.addr_wrap = loops0 == 1 ? 0 : 18,
			},
		},
		/* DRAM command NOP */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_NOP,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = gap1,
				.post_ssq_wait  = 4,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 8,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap = 31,
			},
		},
		/* DRAM command WR */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_WR,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = loops2,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = 4,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = wrap2,
			},
		},
		/* DRAM command NOP */
		[3] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_NOP,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->CWL + ctrl->tWTR + 5,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 8,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap = 31,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_command_training_sequence(
	ramctr_timing *ctrl, int channel, int slotrank, unsigned int address)
{
	const struct iosav_ssq sequence[] = {
		/* DRAM command ACT */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_ACT,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 8,
				.cmd_delay_gap  = MAX(ctrl->tRRD, (ctrl->tFAW >> 2) + 1),
				.post_ssq_wait  = ctrl->tRCD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = address,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_bank  = 1,
				.addr_wrap = 18,
			},
		},
		/* DRAM command WR */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_WR,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 32,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = ctrl->CWL + ctrl->tWTR + 8,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
				.lfsr_upd   = 3,
				.lfsr_xors  = 2,
			},
		},
		/* DRAM command RD */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_RD,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 32,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = MAX(ctrl->tRTP, 8),
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
				.lfsr_upd   = 3,
				.lfsr_xors  = 2,
			},
		},
		/* DRAM command PRE */
		[3] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_PRE,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = 15,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 1 << 10,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap = 18,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_data_write_sequence(ramctr_timing *ctrl, int channel, int slotrank)
{
	const struct iosav_ssq sequence[] = {
		/* DRAM command ACT */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_ACT,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 4,
				.cmd_delay_gap = MAX(ctrl->tRRD, (ctrl->tFAW >> 2) + 1),
				.post_ssq_wait  = ctrl->tRCD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_bank  = 0,
				.addr_wrap = 18,
			},
		},
		/* DRAM command WR */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_WR,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 32,
				.cmd_delay_gap  = 20,
				.post_ssq_wait  = ctrl->CWL + ctrl->tWTR + 8,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
			},
		},
		/* DRAM command RD */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_RD,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 32,
				.cmd_delay_gap  = 20,
				.post_ssq_wait  = MAX(ctrl->tRTP, 8),
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
			},
		},
		/* DRAM command PRE */
		[3] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_PRE,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->tRP,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 1 << 10,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_aggressive_write_read_sequence(ramctr_timing *ctrl, int channel, int slotrank)
{
	const struct iosav_ssq sequence[] = {
		/* DRAM command ACT */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_ACT,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 4,
				.cmd_delay_gap  = MAX((ctrl->tFAW >> 2) + 1, ctrl->tRRD),
				.post_ssq_wait  = ctrl->tRCD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_bank  = 1,
				.addr_wrap = 18,
			},
		},
		/* DRAM command WR */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_WR,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 480,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = ctrl->tWTR + ctrl->CWL + 8,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
			},
		},
		/* DRAM command RD */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_RD,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 480,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = MAX(ctrl->tRTP, 8),
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
			},
		},
		/* DRAM command PRE */
		[3] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_PRE,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = ctrl->tRP,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 1 << 10,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_memory_test_sequence(ramctr_timing *ctrl, int channel, int slotrank)
{
	const struct iosav_ssq sequence[] = {
		/* DRAM command ACT */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_ACT,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 4,
				.cmd_delay_gap  = 8,
				.post_ssq_wait  = 40,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_bank  = 1,
				.addr_wrap = 18,
			},
		},
		/* DRAM command WR */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_WR,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 100,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = 40,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
			},
		},
		/* DRAM command RD */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_RD,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 100,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = 40,
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
			},
		},
		/* DRAM command PRE */
		[3] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_PRE,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = 40,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 1 << 10,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap = 18,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}
