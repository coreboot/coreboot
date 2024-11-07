/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <soc/addressmap.h>
#include <soc/tracker.h>
#include <soc/tracker_common.h>

/*
 * for systracker:
 * offset[0] dump from offset 0x100 ~ 0x13C.
 * offset[1] dump from offset 0x200 ~ 0x23C.
 */

static const u32 offsets[] = { AR_TRACK_OFFSET, AW_TRACK_OFFSET };

struct tracker tracker_data[TRACKER_NUM] = {
	[TRACKER_SYSTRACKER] = {
		.base_addr = DBG_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = SYS_TRACK_ENTRY,
		.offsets = offsets,
		.offsets_size = ARRAY_SIZE(offsets),
		.str = "systracker",
	},
};
