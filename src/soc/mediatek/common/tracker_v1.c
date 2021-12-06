/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <soc/tracker.h>
#include <soc/tracker_common.h>

/*
 * for systracker:
 * offset[0] dump from offset 0x100 ~ 0x13C.
 * offset[1] dump from offset 0x200 ~ 0x23C.
 */
struct tracker tracker_data[TRACKER_NUM] = {
	[TRACKER_SYSTRACKER] = {
		.base_addr = DBG_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = SYS_TRACK_ENTRY,
		.offset[0] = AR_TRACK_OFFSET,
		.offset[1] = AW_TRACK_OFFSET,
		.str = "systracker",
	},
};
