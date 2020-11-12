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

void iosav_run_once(const int ch)
{
	iosav_run_queue(ch, 1, 0);
}

void wait_for_iosav(int channel)
{
	while (1) {
		if (MCHBAR32(IOSAV_STATUS_ch(channel)) & 0x50)
			return;
	}
}

void iosav_write_zqcs_sequence(int channel, int slotrank, u32 gap, u32 post, u32 wrap)
{
	const struct iosav_ssq sequence[] = ZQCS_SEQUENCE(slotrank, gap, post, wrap);
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_prea_sequence(int channel, int slotrank, u32 post, u32 wrap)
{
	const struct iosav_ssq sequence[] = PREA_SEQUENCE(post, wrap);
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_read_mpr_sequence(
	int channel, int slotrank, u32 tMOD, u32 loops, u32 gap, u32 loops2, u32 post2)
{
	const struct iosav_ssq sequence[] = READ_MPR_SEQUENCE(tMOD, loops, gap, loops2, post2);
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_misc_write_sequence(ramctr_timing *ctrl, int channel, int slotrank,
				     u32 gap0, u32 loops0, u32 gap1, u32 loops2, u32 wrap2)
{
	const struct iosav_ssq sequence[] =
		MISC_WRITE_SEQUENCE(gap0, loops0, gap1, loops2, wrap2);
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_command_training_sequence(
	ramctr_timing *ctrl, int channel, int slotrank, unsigned int address)
{
	const struct iosav_ssq sequence[] = COMMAND_TRAINING_SEQUENCE(address);
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_data_write_sequence(ramctr_timing *ctrl, int channel, int slotrank)
{
	const struct iosav_ssq sequence[] = WRITE_DATA_SEQUENCE;
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_aggressive_write_read_sequence(ramctr_timing *ctrl, int channel, int slotrank)
{
	const struct iosav_ssq sequence[] = AGGRESSIVE_WRITE_READ_SEQUENCE;
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}

void iosav_write_memory_test_sequence(ramctr_timing *ctrl, int channel, int slotrank)
{
	const struct iosav_ssq sequence[] = MEMORY_TEST_SEQUENCE;
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));
}
