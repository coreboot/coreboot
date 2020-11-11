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

void iosav_write_ssq(const int ch, const struct iosav_ssq *ssq)
{
	MCHBAR32(IOSAV_n_SP_CMD_CTRL_ch(ch, ssq_count)) = ssq->sp_cmd_ctrl.raw;
	MCHBAR32(IOSAV_n_SUBSEQ_CTRL_ch(ch, ssq_count)) = ssq->subseq_ctrl.raw;
	MCHBAR32(IOSAV_n_SP_CMD_ADDR_ch(ch, ssq_count)) = ssq->sp_cmd_addr.raw;
	MCHBAR32(IOSAV_n_ADDR_UPDATE_ch(ch, ssq_count)) = ssq->addr_update.raw;

	ssq_count++;
}

void iosav_run_queue(const int ch, const u8 loops, const u8 as_timer)
{
	MCHBAR32(IOSAV_SEQ_CTL_ch(ch)) = loops | ((ssq_count - 1) << 18) | (as_timer << 22);

	ssq_count = 0;
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
